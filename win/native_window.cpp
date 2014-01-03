//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_window.cpp
// copyright:   (C) 2008, 2009, 2014 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <Windowsx.h>   // GET_X_LPARAM()/GET_Y_LPARAM()

#include "app_window.h"
#include "capture.h"
#include "native_window.h"
#include "options.h"
#include "resource.h"   // windows only
//-----------------------------------------------------------------------------
const char* win_class_name="MatrixRainWinClass";
//-----------------------------------------------------------------------------
NativeWindow::NativeWindow()
    : hwnd(0)
{
}

NativeWindow::~NativeWindow()
{
    DestroyWindow(hwnd);
    UnregisterClass(win_class_name, GetModuleHandle(NULL));
}

bool NativeWindow::process_events()
{
    MSG msg;
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT) return false;

        TranslateMessage(&msg);
        DispatchMessage (&msg);
    }
    return true;
}

void NativeWindow::create_window(AppWindow* win, unsigned int parent_id, int width, int height)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc;
    wc.style        = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_DBLCLKS;
    wc.lpfnWndProc  = (WNDPROC)ScreenSaverProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 8;
    wc.hInstance    = hInstance;
    wc.hIcon        = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor      = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground= NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName= win_class_name;

    RegisterClass(&wc);

    // default properties
    DWORD exstyle = WS_EX_APPWINDOW;
    DWORD style   = WS_OVERLAPPEDWINDOW;
    HWND parent = NULL;
    int x = 0;
    int y = 0;

    if( AppWindow::preview == win->mode && 0 != parent_id )
    {
        exstyle = 0;
        style   = WS_CHILD;
        parent = (HWND)parent_id;
        RECT rc={0};
        GetWindowRect(parent, &rc);

        width  = rc.right-rc.left;
        height = rc.bottom-rc.top;
    }
    else
    {
        RECT desktop={0};
        GetWindowRect(GetDesktopWindow(), &desktop);

        if( AppWindow::screensaver == win->mode )
        {
            exstyle= WS_EX_TOPMOST;
            style  = WS_POPUP;
            width  = desktop.right;
            height = desktop.bottom;

            ShowCursor(FALSE);
            SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, 1, NULL, SPIF_SENDCHANGE);
        }
        else if(AppWindow::standalone == win->mode)
        {
            x = (desktop.right - width)/2;
            y = (desktop.bottom- height)/2;
        }
    }

//Thremble
    hwnd = CreateWindowEx(exstyle, win_class_name, AppWindow::caption, style,
                            x, y, width, height, parent, NULL, hInstance, win);
}

void NativeWindow::activate()
{
    ShowWindow(hwnd, SW_SHOW);
}

LRESULT NativeWindow::ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    AppWindow* win = (AppWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch(msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs=(CREATESTRUCT*)lParam;
            win = (AppWindow*)cs->lpCreateParams;
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)win);
            GetCursorPos(&win->start_point);
            SetTimer(hWnd, BREAK_TIMER_ID, MAX_EVENTS_PROCESSING_TIME, NULL );
            win->hwnd = hWnd;
            win->create_renderer();
        }
        break;

        case WM_DESTROY:
        KillTimer(win->hwnd, BREAK_TIMER_ID);
        if( AppWindow::screensaver == win->mode )
        {
            SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, 0, NULL, SPIF_SENDCHANGE);
            ShowCursor(TRUE);
        }
        break;

        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
    //  case WM_NCACTIVATE:
        if(FALSE == wParam && win->mode == AppWindow::screensaver) PostQuitMessage(0);
        break;

        case WM_SIZE:
            win->resize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_ERASEBKGND:
            win->repaint();
            return TRUE;
        break;

        case WM_PAINT:
            win->repaint();
            ValidateRect(win->hwnd, NULL);
            return FALSE;
        break;

        case WM_MOVING:
        if(win->mode == AppWindow::standalone) return TRUE;
        break;

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if(win->start_point.x == x && win->start_point.y == y) break;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_KEYDOWN:

        if(wParam != VK_ESCAPE && win->mode != AppWindow::screensaver) break;
        case WM_CLOSE:
            win->destroy_renderer();
            PostQuitMessage(0);
        break;

        case WM_TIMER:
            win->repaint();
        break;

        case WM_SYSCOMMAND:
        if( SC_SCREENSAVE == wParam )
        {
            return FALSE;
        }
        else if( SC_MONITORPOWER == wParam ) // Monitor Trying To Enter Powersave?
        {
            return 0;// Prevent From Happening
        }

        default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

Config::Config(Options& opts)
    : options(opts)
{
}

bool Config::show_dialog(int parent_id)
{
    // Special case -1: use Desktop window as a parent
    if(-1 == parent_id) parent_id = (int)GetDesktopWindow();
    return DialogBoxParam(NULL, MAKEINTRESOURCE(IDD_CONFIG), (HWND)parent_id, DialogProc, (LPARAM)this) < 1;
}

INT_PTR CALLBACK Config::DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Config* cfg = (Config*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            SetWindowLongPtr(hwnd, GWLP_USERDATA,(LONG_PTR)lParam);
            cfg = (Config*)lParam;

            HWND parent = GetParent(hwnd);
            if(0 == parent) parent = GetDesktopWindow();

            // Set Position
            RECT parent_size={0};
            RECT my_size={0};
            GetWindowRect(parent, &parent_size);
            GetWindowRect(hwnd  , &my_size);

            LONG x = (parent_size.right - (my_size.right-my_size.left))>>1;
            LONG y = (parent_size.bottom - (my_size.bottom-my_size.top))>>1;
            SetWindowPos(hwnd, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

            // Initialize controls
            char buffers[5][128]={0};
            unsigned int count = Capture::enum_devices(buffers, 5);
            if(0 == count)
            {
                int id = (int)SendDlgItemMessage(hwnd, IDC_DEVICE, CB_ADDSTRING, 0, (LPARAM)"Capture Device not found");
                SendDlgItemMessage(hwnd, IDC_DEVICE, CB_SETITEMDATA, id, -1);
                SendDlgItemMessage(hwnd, IDC_DEVICE, CB_SETCURSEL, id, 0);
                EnableWindow(GetDlgItem(hwnd, IDC_DEVICE), FALSE);
            }
            else
            {
                const char* configured_device = cfg->options[Options::device];
                if(configured_device == NULL || *configured_device == '\0')
                {
                    // if not set - take first device
                    configured_device = buffers[0];
                }

                for(unsigned int i=0; i<count; i++)
                {
                    int id = (int)SendDlgItemMessage(hwnd, IDC_DEVICE, CB_ADDSTRING, 0,(LPARAM)buffers[i]);
                    SendDlgItemMessage(hwnd, IDC_DEVICE, CB_SETITEMDATA, id, i);
                    if( 0 == strcmp(configured_device, buffers[i]) )
                    {
                        SendDlgItemMessage(hwnd, IDC_DEVICE, CB_SETCURSEL, id, 0);
                    }
                }
            }

            SendDlgItemMessage(hwnd, IDC_NOSHADERS, BM_SETCHECK, cfg->options[Options::no_shaders] ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(hwnd, IDC_VFLIP,     BM_SETCHECK, cfg->options[Options::vflip] ? BST_CHECKED : BST_UNCHECKED, 0);
            SendDlgItemMessage(hwnd, IDC_HFLIP,     BM_SETCHECK, cfg->options[Options::hflip] ? BST_CHECKED : BST_UNCHECKED, 0);

            //SetDlgItemText(hwnd, IDC_AKEY, cfg->options[Options::key]);
        }
        return (INT_PTR)TRUE;

        case WM_COMMAND:
            switch(LOWORD(wParam))
            {
                case IDC_OK:
                {
                    int id = (int)SendDlgItemMessage(hwnd, IDC_DEVICE, CB_GETCURSEL, 0, 0);
                    int index = (int)SendDlgItemMessage(hwnd, IDC_DEVICE, CB_GETITEMDATA, id, 0);
                    // save options
                    char buffer[128]={0};
                    if( GetDlgItemText(hwnd, IDC_DEVICE, buffer, sizeof(buffer)) )
                    {
                        cfg->options[Options::device] = buffer;
                    }
                    cfg->options[Options::no_shaders] = BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_NOSHADERS);
                    cfg->options[Options::vflip] = BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_VFLIP);
                    cfg->options[Options::hflip] = BST_CHECKED == IsDlgButtonChecked(hwnd, IDC_HFLIP);
                    cfg->options.save();
                    EndDialog(hwnd, TRUE);
                    return (INT_PTR)TRUE;
                }
                break;

                case IDC_CANCEL:
                {
                    EndDialog(hwnd, FALSE);
                    return (INT_PTR)TRUE;
                }
                break;
            };

        break;
        case WM_CLOSE:
            EndDialog(hwnd, FALSE);
            return (INT_PTR)TRUE;
        break;
    }
    return (INT_PTR)FALSE;
}
//-----------------------------------------------------------------------------
