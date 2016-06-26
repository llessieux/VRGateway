#include "stdafx.h"
#include "VRGateway.h"
#include "taikoVRApp.h"
#include "PataponVRApp.h"
#include "VLCVRApp.h"

#define MAX_LOADSTRING 100
//#define VR_DISABLED

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];            

std::unique_ptr<TaikoVRApp> gtaikoVRApp;
std::unique_ptr<PataponVRApp> gpataponVRApp;
std::unique_ptr<VLCVRApp> gVLCVRApp;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, LPWSTR );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// d3d function prototypes

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_VRGATEWAY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow, lpCmdLine))
    {
        return FALSE;
    }

    //HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VRGATEWAY));

    MSG msg;
    bool done;


    // Initialize the message structure.
    ZeroMemory(&msg, sizeof(MSG));

    // Loop until there is a quit message from the window or the user.
    done = false;
    while (!done)
    {
        // Handle the windows messages.
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // If windows signals to end the application then exit out.
        if (msg.message == WM_QUIT)
        {
            done = true;
        }
        else
        {
            // Otherwise do the frame processing.
            if (gtaikoVRApp.get())
                gtaikoVRApp->render_frame();
            if (gpataponVRApp.get())
                gpataponVRApp->render_frame();
            if (gVLCVRApp.get())
                gVLCVRApp->render_frame();
        }

    }

    if (gtaikoVRApp.get())
        gtaikoVRApp.release();
    if (gpataponVRApp.get())
        gpataponVRApp.release();
    if (gVLCVRApp.get())
        gVLCVRApp.release();

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VRGATEWAY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_VRGATEWAY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, LPWSTR cmdLine)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, VRCLIENTWIDTH, VRCLIENTHEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   //SetWindowPos(hWnd, 0, 1920, 1080, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
   UpdateWindow(hWnd);

   if (wcsstr(cmdLine, L"-psp_taiko") != nullptr)
   {
       gtaikoVRApp.reset(new TaikoVRApp());
       if (!gtaikoVRApp->init(hWnd))
       {
           exit(0);
       }
       char txt[256];
       GetPrivateProfileStringA("PPSSPP", "PATH", "", txt, 256, ".\\TaikoVR.INI");
       std::string ppssppPath(txt);
       std::string separator("\\");
       if (ppssppPath.back() == '\\')
           separator.clear();
       std::string iniPath = ppssppPath + separator + "memstick\\PSP\\SYSTEM\\controls.ini";
       gtaikoVRApp->ReadPSPControlSettings(iniPath.c_str());
   }
   else if (wcsstr(cmdLine, L"-psp_patapon") != nullptr)
   {
       gpataponVRApp.reset(new PataponVRApp());
       if (!gpataponVRApp->init(hWnd))
       {
           exit(0);
       }
       char txt[256];
       GetPrivateProfileStringA("PPSSPP", "PATH", "", txt, 256, ".\\TaikoVR.INI");
       std::string ppssppPath(txt);
       std::string separator("\\");
       if (ppssppPath.back() == '\\')
           separator.clear();
       std::string iniPath = ppssppPath + separator + "memstick\\PSP\\SYSTEM\\controls.ini";
       gpataponVRApp->ReadPSPControlSettings(iniPath.c_str());
   }
   else if (wcsstr(cmdLine, L"-vlc_vr") != nullptr)
   {
       bool stereo_input = wcsstr(cmdLine, L"-vlc_stereo_vr") != nullptr;
       gVLCVRApp.reset(new VLCVRApp(stereo_input));
       if (!gVLCVRApp->init(hWnd))
       {
           exit(0);
       }
   }
   else
       return FALSE;

   return TRUE;
}
//
//  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  WM_COMMAND  
//  WM_PAINT    
//  WM_DESTROY  
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


