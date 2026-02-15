#define UNICODE
#define _UNICODE

#define WM_SPAWN_WORKERW 0x052C // this code sent the message to explorer to make WorkerW!

#include <windows.h>
#include <iostream>

// this is globle because i want simple code and if a Veribal's have (g_) than dont change tham if you dont know what you doing!!!
HWND g_WorkerW = NULL;
 
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK EnumWindowprocWin10(HWND hwnd, LPARAM lparam);


int WINAPI wWinMain(HINSTANCE hInstance,
    HINSTANCE hprevInstance,
    PWSTR pCmdLine,
    int nCmdshow)
{
    SetProcessDpiAwarenessContext(
        DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    // Find Program! if NULL exit the code
    HWND progman = FindWindow(L"Progman", NULL);
    if (progman == NULL) {
        MessageBox(NULL, L"Progman NOT found", L"Error", MB_ICONERROR);
        return 0;
    }

    // if Progman found than sent the message tothe explorer to make WorkerW!
    // and iam indeed senting all type of Message to program just to make sure thats it!
    SendMessage(progman, WM_SPAWN_WORKERW, 0, 0);
    Sleep(20); // just to not spam program in case
    SendMessage(progman, WM_SPAWN_WORKERW, 0xD, 0);
    Sleep(20); // just to not spam program in case
    SendMessage(progman, WM_SPAWN_WORKERW, 0xD, 1);
    Sleep(100); // just to not spam program in case

    //TRY to find workerW set the max limit to 5 between 10 5 is enough!
    for (int i = 0; i < 5 && g_WorkerW == NULL; i++)
    {
        EnumWindows(EnumWindowsProc, 0);
        Sleep(50);
    }

    //chack if wokerW found or not! if not than we try deferent Method! 
    if (g_WorkerW == NULL)
    { 
        EnumWindows(EnumWindowprocWin10, 0);
        
        if(g_WorkerW == NULL)
        {
            MessageBox(NULL, L"WorkerW not found!", L"Error", MB_ICONERROR);
            return 0;
        }
    }

    // ALright if we found the workerW than we start makeing ourwindow than Attach to workerW window!!
    WNDCLASSEX wc = { };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.lpszClassName = L"MyWallpaperWindow";
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassEx(&wc);

    HWND mywindow = CreateWindowEx(WS_EX_LAYERED |
        WS_EX_TRANSPARENT |
        WS_EX_NOACTIVATE |
        WS_EX_TOOLWINDOW,
        wc.lpszClassName,
        L"",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!mywindow) {
        MessageBox(NULL, L"CreateWindowEx failed", L"Error", MB_ICONERROR);
        return 0;
    }
    else
    {
        SetParent(mywindow,g_WorkerW);

        RECT rc;
        GetClientRect(g_WorkerW, &rc);

        SetWindowPos(
            mywindow,
            NULL,
            0, 0,
            rc.right - rc.left,
            rc.bottom - rc.top,
            SWP_NOZORDER | SWP_NOACTIVATE
        );

        ShowWindow(mywindow, SW_SHOW);
        SetLayeredWindowAttributes(mywindow, 0, 255, LWA_ALPHA); // we will remove on DX get added!
        UpdateWindow(mywindow);
        MessageBox(NULL, L"workerW and CreateWindowEX done!!", L"Success!", MB_ICONINFORMATION);
    }

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// this funtion find the WorkerW window and dont change the code here it's Sensitive if nothink is break!! 
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lparam)
{
    (void)lparam; // just to shut up Warnings!

    HWND progman = FindWindow(L"Progman", NULL);
    if (progman == NULL) {
        MessageBox(NULL, L"Progman NOT found", L"Error", MB_ICONERROR);
        return TRUE;
    }

    HWND maybeworkerW = FindWindowExW(progman,   NULL, L"WorkerW", NULL);
    
    HWND shelldll_defview = FindWindowEx(maybeworkerW, NULL, L"SHELLDLL_DefView", NULL);

    if(shelldll_defview != NULL)
    {
        return TRUE; // this workerW contand shelldll view that not we want we want empty one
    }
    else
    {
        g_WorkerW = maybeworkerW;
        return FALSE; // workerW found stop!
    }

    //if(g_WorkerW == NULL)
    //{
    //    MessageBox(NULL,L"workerW is still NULL trying agian!", L"Error", NULL);
    //    return TRUE;
    //}

    return FALSE;
}

// This is for Mainy for Windows 10 why because in Win 10 program dont have any child window Until
// we sent program Message 0x052C than A workerW get makeed but Surprisingly  Progman still dont call
// that worker his Child but that WorkerW DO say Hey iam Progman Child. Confusing right iknow 
// this is why i Write this code. it first look yes  indeed wried but it work if Previous worlerW Finder fail's!
// AND for FYI i cant get hands on other windows version.. i only tested on my PC(WIN11) and my Friend PC(WIN10)!
BOOL CALLBACK EnumWindowprocWin10(HWND hwnd, LPARAM lparam)
{
    HWND progman = FindWindow(L"Progman", NULL);

    wchar_t cls[256];
    GetClassName(hwnd, cls, 256);

    if (wcscmp(cls, L"WorkerW") == 0)
    {
        HWND parent = GetParent(hwnd);

        if (parent == progman)
        {
            MessageBox(NULL, L"found workerW that child of progman", L"Success", MB_ICONINFORMATION);
            g_WorkerW = hwnd;
            return FALSE; // stop we found it
        }

    }


}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
            FillRect(hdc, &rc, redBrush);
            DeleteObject(redBrush); // importand !!

            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, umsg, wparam, lparam);
}
