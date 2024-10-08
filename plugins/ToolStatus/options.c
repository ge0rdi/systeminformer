/*
 * Copyright (c) 2022 Winsider Seminars & Solutions, Inc.  All rights reserved.
 *
 * This file is part of System Informer.
 *
 * Authors:
 *
 *     wj32    2010-2013
 *     dmex    2011-2022
 *
 */

#include "toolstatus.h"

static PH_KEY_VALUE_PAIR GraphTypePairs[] =
{
    { L"None", (PVOID)TASKBAR_ICON_NONE },
    { L"CPU usage", (PVOID)TASKBAR_ICON_CPU_USAGE },
    { L"CPU history", (PVOID)TASKBAR_ICON_CPU_HISTORY },
    { L"I/O history", (PVOID)TASKBAR_ICON_IO_HISTORY },
    { L"Commit charge history", (PVOID)TASKBAR_ICON_COMMIT_HISTORY },
    { L"Physical memory history", (PVOID)TASKBAR_ICON_PHYSICAL_HISTORY },
};

static PWSTR GraphTypeStrings[] =
{
    L"None",
    L"CPU usage",
    L"CPU history",
    L"I/O history",
    L"Commit charge history",
    L"Physical memory history"
};

PWSTR GraphTypeGetTypeString(
    _In_ ULONG SidType
    )
{
    PWSTR string;

    if (PhFindStringSiKeyValuePairs(
        GraphTypePairs,
        sizeof(GraphTypePairs),
        SidType,
        &string
        ))
    {
        return string;
    }

    return L"None";
}

ULONG GraphTypeGetTypeInteger(
    _In_ PWSTR SidType
    )
{
    ULONG integer;

    if (PhFindIntegerSiKeyValuePairs(
        GraphTypePairs,
        sizeof(GraphTypePairs),
        SidType,
        &integer
        ))
    {
        return integer;
    }

    return 0;
}

INT_PTR CALLBACK OptionsDlgProc(
    _In_ HWND WindowHandle,
    _In_ UINT WindowMessage,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
    )
{
    switch (WindowMessage)
    {
    case WM_INITDIALOG:
        {
            HWND graphTypeHandle;

            Button_SetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_TOOLBAR), ToolStatusConfig.ToolBarEnabled ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_STATUSBAR), ToolStatusConfig.StatusBarEnabled ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(GetDlgItem(WindowHandle, IDC_RESOLVEGHOSTWINDOWS), ToolStatusConfig.ResolveGhostWindows ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_AUTOHIDE_MENU), ToolStatusConfig.AutoHideMenu ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_AUTOFOCUS_SEARCH), ToolStatusConfig.SearchAutoFocus ? BST_CHECKED : BST_UNCHECKED);
            Button_SetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_LARGETOOLBARICON), ToolStatusConfig.ToolBarLargeIcons ? BST_CHECKED : BST_UNCHECKED);

            graphTypeHandle = GetDlgItem(WindowHandle, IDC_CURRENT);
            PhAddComboBoxStrings(graphTypeHandle, GraphTypeStrings, RTL_NUMBER_OF(GraphTypeStrings));
            PhSelectComboBoxString(graphTypeHandle, GraphTypeGetTypeString(PhGetIntegerSetting(SETTING_NAME_TASKBARDISPLAYSTYLE)), FALSE);
        }
        break;
    case WM_DESTROY:
        {
            PPH_STRING graphTypeString;

            ToolStatusConfig.ToolBarEnabled = Button_GetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_TOOLBAR)) == BST_CHECKED;
            ToolStatusConfig.StatusBarEnabled = Button_GetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_STATUSBAR)) == BST_CHECKED;
            ToolStatusConfig.ResolveGhostWindows = Button_GetCheck(GetDlgItem(WindowHandle, IDC_RESOLVEGHOSTWINDOWS)) == BST_CHECKED;
            ToolStatusConfig.AutoHideMenu = Button_GetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_AUTOHIDE_MENU)) == BST_CHECKED;
            ToolStatusConfig.SearchAutoFocus = Button_GetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_AUTOFOCUS_SEARCH)) == BST_CHECKED;
            ToolStatusConfig.ToolBarLargeIcons = Button_GetCheck(GetDlgItem(WindowHandle, IDC_ENABLE_LARGETOOLBARICON)) == BST_CHECKED;

            PhSetIntegerSetting(SETTING_NAME_TOOLSTATUS_CONFIG, ToolStatusConfig.Flags);

            ToolbarLoadSettings(FALSE);
            ToolbarCreateGraphs();

            if (ToolStatusConfig.AutoHideMenu)
            {
                SetMenu(MainWindowHandle, NULL);
            }
            else
            {
                SetMenu(MainWindowHandle, MainMenu);
                DrawMenuBar(MainWindowHandle);
            }

            if (ToolStatusConfig.SearchBoxEnabled && ToolStatusConfig.SearchAutoFocus && SearchboxHandle)
                SetFocus(SearchboxHandle);

            graphTypeString = PH_AUTO(PhGetWindowText(GetDlgItem(WindowHandle, IDC_CURRENT)));
            PhSetIntegerSetting(SETTING_NAME_TASKBARDISPLAYSTYLE, GraphTypeGetTypeInteger(graphTypeString->Buffer));
            TaskbarListIconType = PhGetIntegerSetting(SETTING_NAME_TASKBARDISPLAYSTYLE);
            TaskbarIsDirty = TRUE;

            TaskbarInitialize();

            SendMessage(MainWindowHandle, WM_DPICHANGED, 0, 0);
        }
        break;
    case WM_CTLCOLORBTN:
        return HANDLE_WM_CTLCOLORBTN(WindowHandle, wParam, lParam, PhWindowThemeControlColor);
    case WM_CTLCOLORDLG:
        return HANDLE_WM_CTLCOLORDLG(WindowHandle, wParam, lParam, PhWindowThemeControlColor);
    case WM_CTLCOLORSTATIC:
        return HANDLE_WM_CTLCOLORSTATIC(WindowHandle, wParam, lParam, PhWindowThemeControlColor);
    }

    return FALSE;
}
