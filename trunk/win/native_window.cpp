//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_window.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <stdio.h>

#include "capture.h"
#include "native_window.h"
//-----------------------------------------------------------------------------
const char* NativeWindow::win_class_name="MatrixRainWinClass";

NativeWindow::NativeWindow(BaseWindow::Mode mode, unsigned int parent_id, int width, int height):BaseWindow(mode),hwnd(0)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    WNDCLASS wc;
    wc.style        = CS_VREDRAW | CS_HREDRAW | CS_SAVEBITS | CS_DBLCLKS;
    wc.lpfnWndProc  = (WNDPROC)ScreenSaverProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = 8;
    wc.hInstance    = hInstance;
    wc.hIcon        = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor      = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground= (HBRUSH) GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= NativeWindow::win_class_name;

    RegisterClass(&wc);

    // default properties
    DWORD exstyle = WS_EX_APPWINDOW;
    DWORD style   = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    HWND parent = NULL;
    int x = 0;
    int y = 0;

    if( BaseWindow::preview == mode && 0 != parent_id )
    {
        exstyle = 0;
        style   = WS_CHILD|WS_VISIBLE;
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

        if( BaseWindow::screensaver == mode )
        {
            //      SetCursor(NULL);
            exstyle= WS_EX_TOPMOST;
            style  = WS_POPUP|WS_VISIBLE;
            width  = desktop.right;
            height = desktop.bottom;

            ShowCursor(FALSE);
            SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, 1, NULL, SPIF_SENDCHANGE);
        }
        else if(BaseWindow::standalone == mode)
        {
            x = (desktop.right - width)>>1;
            y = (desktop.bottom- height)>>1;
        }
    }

//Thremble
    hwnd = CreateWindowEx(exstyle, NativeWindow::win_class_name, BaseWindow::caption, style,
                            x, y, width, height, parent, NULL, hInstance, this);
    if( 0 == SetTimer(hwnd, BREAK_TIMER_ID, MAX_EVENTS_PROCESSING_TIME, NULL ) )
    {
        printf("Break timer not set !");
    }
}

NativeWindow::~NativeWindow()
{
    if( BaseWindow::screensaver == mode )
    {
        SystemParametersInfo(SPI_SETSCREENSAVERRUNNING, 0, NULL, SPIF_SENDCHANGE);
        ShowCursor(TRUE);
    }
    KillTimer(hwnd, BREAK_TIMER_ID);
    SetWindowLong(hwnd, GWL_USERDATA, 0);
    DestroyWindow(hwnd);
    UnregisterClass(NativeWindow::win_class_name, GetModuleHandle(NULL));
}

bool NativeWindow::process_events()
{
    MSG msg;
    if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        if(msg.message == WM_QUIT) return false;
        // break too long event processing
     /*   if((WM_TIMER == msg.message) && (BREAK_TIMER_ID == msg.wParam))
        {
            return true;
        }*/
        TranslateMessage(&msg);
        DispatchMessage (&msg);
 
    }
    return true;
}

void NativeWindow::get_size(unsigned int* width, unsigned int* height)const
{
    RECT rc={0};
    GetWindowRect(hwnd, &rc);

    *width  = rc.right-rc.left;
    *height = rc.bottom-rc.top;
}

LONG NativeWindow::ScreenSaverProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    NativeWindow* win = (NativeWindow*)GetWindowLong(hWnd, GWL_USERDATA);

    switch(msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *cs=(CREATESTRUCT*)lParam;
            win = (NativeWindow*)cs->lpCreateParams;
            SetWindowLong(hWnd, GWL_USERDATA,(LONG)win);
            GetCursorPos(&win->start_point);
        }
        break;

        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
    //  case WM_NCACTIVATE:
        if(FALSE == wParam && win && win->mode == BaseWindow::screensaver) PostQuitMessage(0);
        break;

        case WM_SIZE:
            win->resize(LOWORD(lParam), HIWORD(lParam));
        break;

        case WM_MOUSEMOVE:
        {
            if(win == 0) break;
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            if(win->start_point.x == x && win->start_point.y == y) break;
        }
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_KEYDOWN:

        if(wParam != VK_ESCAPE && win->mode != BaseWindow::screensaver) break;
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

 /*       case WM_TIMER:
        printf("WM_TIMER\n");
        break;
        */
        case WM_PAINT:
          //  win->paint();
        break;

        case WM_MOVING:
        if(win->mode == BaseWindow::standalone) return TRUE;
        break;

/*      case WM_SETCURSOR:
        SetCursor(NULL);
        ShowCursor(FALSE);
        return TRUE;
*/
//      case WM_DESTROY: PostMessage(hWnd, WM_CLOSE, 0, 0); break;

        case WM_SYSCOMMAND:
        if( SC_SCREENSAVE == wParam )
            return FALSE;

        default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

Config::Config(Options& opts):options(opts),registration(false)
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
    Config* cfg = (Config*)GetWindowLong(hwnd, GWL_USERDATA);

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            SetWindowLong(hwnd, GWL_USERDATA,(LONG)lParam);
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
                SendDlgItemMessage(hwnd, IDC_DEVICE, CB_ADDSTRING, 0, (LPARAM)"Capture Device not found");
                SendDlgItemMessage(hwnd, IDC_DEVICE, CB_SETCURSEL, 0, 0);
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
                SendDlgItemMessage(hwnd, IDC_DEVICE, CB_ADDSTRING, 0, (LPARAM)configured_device);
                SendDlgItemMessage(hwnd, IDC_DEVICE, CB_SETCURSEL, 0, 0);

                for(unsigned int i=0; i<count; i++)
                {
                    if( 0 == strcmp(configured_device, buffers[i]) ) continue;
                    SendDlgItemMessage(hwnd, IDC_DEVICE, CB_ADDSTRING, 0,(LPARAM)buffers[i]);
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

                case IDC_REGISTER:
                {
              /*      RECT size={0};
                    GetWindowRect(hwnd, &size);
                    int cx = size.right - size.left;
                    int cy = size.bottom - size.top;

                    if(!cfg->registration)   // maximize window
                    {
                        cfg->registration = true;
                        cy += 150;
                    }
                    else
                    {
                        cfg->registration = false;
                        cy -= 150;
                    }
                    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, cx, cy, SWP_NOMOVE);*/
                    return (INT_PTR)TRUE;
                }
                break;

                case IDC_SUBMIT:
                {
                    char rkey[128]="rkey";
                    char url[512];
                    _snprintf(url, sizeof(url),"http://mrain.sourceforge.net/activate.php?%s", rkey);
                    ShellExecute(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
                    return (INT_PTR)TRUE;
                }
                break;

                case IDC_AKEY:
                {
                    char buffer[128];
                    UINT count = GetDlgItemText(hwnd, IDC_AKEY, buffer, sizeof(buffer));
                    EnableWindow(GetDlgItem(hwnd, IDC_ACTIVATE), count == License::AKey::key_strlen ? TRUE : FALSE);
                    return (INT_PTR)TRUE;
                }
                break;

                case IDC_ACTIVATE:
                {
/*                    if(cfg->registration)   // minimize window
                    {
                        cfg->registration = false;
                        RECT size={0};
                        GetWindowRect(hwnd, &size);
                        int cx = size.right - size.left;
                        int cy = (size.bottom - size.top) - 150;
                        SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, cx, cy, SWP_NOMOVE);

                        char buffer[License::AKey::key_strlen];
                        if( License::AKey::key_strlen == GetDlgItemText(hwnd, IDC_AKEY, buffer, sizeof(buffer)) )
                        {
                            cfg->options[Options::key] = buffer;
                            cfg->options.save();

                            MessageBox(hwnd, "Your instance of application is registered.", "Successed", MB_OK | MB_ICONINFORMATION);
                        }
                    }*/
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
