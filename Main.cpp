// Strip out unnecessary crap
#define WIN32_LEAN_AND_MEAN

// Global includes.
#include <windows.h>
#include <commdlg.h>
#include <commctrl.h>
#include <d3d9.h>

// Local includes.
#include "resource.h"
#include "dialogs.h"
#include "main.h"

#pragma comment (lib, "d3d9.lib")

void WINAPI InitCommonControlsEx(void);  // Have to do this. Or we don't get to use controls.

// Global variables. Limit the usage of these or experience a huge overhead.
char szClassName[ ] = "Sexy68k";
char ROM_Path[MAX_PATH];
char ini_path[MAX_PATH];
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Function prototypes.
void init3d(HWND hwnd);
void frame(void);
void die(void);

// Create the surface display for the game.
void init3d(HWND hwnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed      = true;
	d3dpp.SwapEffect    = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;

	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
}

// Here's where the action takes place.
void frame(void)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(128, 128, 255), 1.0f, 0);

	d3ddev->BeginScene();
// Frame rendering goes in this space
	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}

// Perform cleanup and release.
void die(void)
{
	d3ddev->Release();
	d3d->Release();
}



// Save emulator settings using an ini file.
//int save_settings(HINSTANCE hInst)
//{
//	{
//	char *p;
//	GetModuleFileName(hInst, ini_path, sizeof(ini_path));
//	p = strrchr(ini_path, '\\');
//	if(NULL == p)
//	{
//		p = (ini_path - 1);
//	}
//
//	strcpy((p + 1), "sexy68k.ini");
//	}
//
//	WritePrivateProfileString
//}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd; 
    MSG msg;           
    WNDCLASSEX wc;       

	// Register the Window. If it fails, all hell breaks loose.
    wc.hInstance     = hInst;
    wc.lpszClassName = szClassName;
    wc.style         = CS_DBLCLKS;                
    wc.cbSize        = sizeof (WNDCLASSEX);  
    wc.hIcon         = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICO));
    wc.hIconSm       = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICO), IMAGE_ICON, 16, 16, 0); // Have to use LoadImage() because LoadIcon() only supports 32x32 icons.
    wc.hCursor       = LoadCursor (NULL, IDC_ARROW);																// Default cursor.
    wc.lpszMenuName  = MAKEINTRESOURCE(IDM_MAINMENU);																// Yay! A menu! :D
    wc.cbClsExtra    = 0;                     
    wc.cbWndExtra    = 0;                     
    wc.hbrBackground = (HBRUSH) COLOR_BACKGROUND +1;
	wc.lpfnWndProc   = WndProc;																						// Pointer to the Window Procedure.

    if (!RegisterClassEx (&wc))
	{
		MessageBox(NULL, "Unable to register window.", "Sexy68k", MB_ICONEXCLAMATION | MB_OK);
        return 0;
	}

	// Succesfully registered. Now let's actually create the window!
    hwnd = CreateWindowEx (
           WS_EX_CLIENTEDGE,  
           szClassName,         
           "Sexy68k",           
           WS_BORDER | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
           CW_USEDEFAULT,     
           CW_USEDEFAULT,       
           800,                 
           600,                 
           HWND_DESKTOP,        
           NULL,
           hInst,
		   NULL
           );
	
	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window creation failed.", "Sexy68k", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
   
	//Pretty standard stuff here.
    ShowWindow (hwnd, nCmdShow);
	init3d(hwnd);
	UpdateWindow(hwnd);

  // Main message loop.
    while (GetMessage (&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
		frame();
    }
	die();

    return msg.wParam;
}



LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)              
    {
		case WM_CREATE: // Statusbars and shit.
        {
            int statwidths[] = {400, -1};
             
            HWND hStatus = CreateWindowEx(
                0,
                STATUSCLASSNAME,
                NULL,
				WS_CHILD | WS_VISIBLE,
                0,
                0,
                0,
                0,
				hwnd,
                (HMENU)IDC_STATUS,
                GetModuleHandle(NULL),
                NULL
                );

           SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
           SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Disk drive 1 is empty.");
		   SendMessage(hStatus, SB_SETTEXT, 1, (LPARAM)"Disk drive 2 is empty.");
		}
        break;

        case WM_SIZE: // Let Windows size them automagically.
        {
			HWND hStatus;
			RECT rcStatus;
			int iStatusHeight;

			SendMessage(GetDlgItem(hwnd, IDC_STATUS), WM_SIZE, 0, 0);

			GetWindowRect(hStatus, &rcStatus);
			iStatusHeight = rcStatus.bottom - rcStatus.top;
        }
        break;

        case WM_COMMAND: // Makes the menu items actually do crap. :o
        switch(LOWORD(wParam))
        {
            case ID_DISKDRIVE1_INSERTDISK:
            DoROMLoad1(hwnd);
            break;

            case ID_DISKDRIVE2_INSERTDISK:
            DoROMLoad2(hwnd);
            break;

			case ID_HELP_ABOUT:
			{
			int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlg);
			if(ret == IDOK)
            {	
               EndDialog(hwnd, IDOK);
            }
            else if(ret == -1)
            {
                MessageBox(hwnd, "Cannot display dialog.", "Sexy68k", MB_OK | MB_ICONINFORMATION);
            }
			}
			break;

            case ID_CONFIG_PATHS:
			{
            int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PATHSCFG), hwnd, PathSetDlg);
            if(ret == IDOK)
            {
               EndDialog(hwnd, IDOK);
            }
            else if(ret == IDCANCEL)
            {
               EndDialog(hwnd, IDCANCEL);
            }
            else if(ret == -1)
            {
                MessageBox(hwnd, "Cannot display dialog.", "Sexy68k", MB_OK | MB_ICONINFORMATION);
            }
			}
			break;

			case ID_INPUT_INPUTOPTIONS:
			{
			int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_INPUTCFG), hwnd, InputCfg);
			if(ret == IDOK)
            {
               EndDialog(hwnd, IDOK);
            }
            else if(ret == IDCANCEL)
            {
               EndDialog(hwnd, IDCANCEL);
            }
            else if(ret == -1)
            {
                MessageBox(hwnd, "Cannot display dialog.", "Sexy68k", MB_OK | MB_ICONINFORMATION);
            }
			}
			break;

			case ID_VIDEO_VIDEOOPTIONS:
			{
			int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_VIDCFG), hwnd, VideoCfg);
			if(ret == IDOK)
            {
               EndDialog(hwnd, IDOK);
            }
            else if(ret == IDCANCEL)
            {
               EndDialog(hwnd, IDCANCEL);
            }
            else if(ret == -1)
            {
                MessageBox(hwnd, "Cannot display dialog.", "Sexy68k", MB_OK | MB_ICONINFORMATION);
            }
			}
			break;

		case ID_FILE_EXIT:
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;		
        }
		break;

        case WM_DESTROY: // Click the little X icon in the window to see what happens.
            PostQuitMessage (0);       
            break;
        default:  // For messages we don't deal with.        
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}