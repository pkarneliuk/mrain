//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_window.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdlib>
#include <string>
#include <stdexcept>

#include <X11/Xatom.h>
#include <GL/glx.h>

#include "native_window.h"
#include "gl_context.h"
//-----------------------------------------------------------------------------
NativeWindow::NativeWindow(BaseWindow::Mode mode, unsigned int id, int width, int height):BaseWindow(mode),display(NULL), window(0)
{
    XSetErrorHandler(NativeWindow::x_error_handler);

    if( !(display = XOpenDisplay(NULL)) )
    {
        throw std::runtime_error("X Server default display is NULL");
    }

    int	screen_num = DefaultScreen(display);
    Screen* screen_ptr = DefaultScreenOfDisplay(display);

    XA_WM_PROTOCOLS		= XInternAtom (display, "WM_PROTOCOLS", False);
    XA_WM_DELETE_WINDOW	= XInternAtom (display, "WM_DELETE_WINDOW", False);

    if( BaseWindow::standalone == mode )
    {
        Visual* visual = GLContext::choose_format(display, screen_num);

        if( !GLContext::check_support(screen_ptr, "window", visual) )
        {
            throw std::runtime_error("X Server does not support this visual format");
        }

        Colormap cmap = XCreateColormap (display, NativeWindow::root_window(screen_ptr), visual, AllocNone);

        XVisualInfo vi_in;
        int out_count;
        vi_in.screen = DefaultScreen(display);
        vi_in.visualid = XVisualIDFromVisual (visual);
        XVisualInfo* vi_out = XGetVisualInfo (display, VisualScreenMask | VisualIDMask, &vi_in, &out_count);
        int visual_depth = vi_out->depth;
        XFree ((char *) vi_out);

        unsigned int display_width  = DisplayWidth (display, screen_num);
        unsigned int display_height = DisplayHeight(display, screen_num);

        int x = (display_width  - width )>>1;
        int y = (display_height - height)>>1;

        XSetWindowAttributes attributes;
        attributes.background_pixel = 0x00000000;   // black
        attributes.border_pixel     = 0x00000000;   // black
        attributes.colormap         = cmap;
        attributes.event_mask       = ButtonPressMask   |
                                      ButtonReleaseMask |
                                      KeyPressMask      |
                                      KeyReleaseMask    |
                                      StructureNotifyMask;

        window = XCreateWindow (display, RootWindow(display, screen_num), x, y,
                                width, height, 0, visual_depth, InputOutput, 
                                visual, CWBackPixel   | 
                                        CWBorderPixel |
                                        CWColormap    |
                                        CWEventMask, &attributes);

        XTextProperty window_name;
        window_name.value = (unsigned char *) BaseWindow::caption;
        window_name.encoding = XA_STRING;
        window_name.format = 8;
        window_name.nitems = sizeof BaseWindow::caption;
        XSetWMName (display, window, &window_name);

        XMapWindow(display, window);
        XMoveWindow(display, window, x, y);

        XChangeProperty(display, window, XA_WM_PROTOCOLS, XA_ATOM, 32, PropModeReplace, (unsigned char*) &XA_WM_DELETE_WINDOW, 1);
        XEvent event;
        XIfEvent (display, &event, map_notify_event, (XPointer) window);  // wait for it to be mapped
    }
    else
    {
        if( BaseWindow::preview == mode && 0 != id )
        {
            window = (Window) id;
        }
        else if( BaseWindow::screensaver == mode )
        {
            window = NativeWindow::root_window(screen_ptr);
        }
        XWindowAttributes xgwa;
        XGetWindowAttributes (display, window, &xgwa);
        //visual_warning (xgwa.screen, window, xgwa.visual, xgwa.colormap, True);

        xgwa.your_event_mask |= KeyPressMask | StructureNotifyMask;
        XSelectInput (display, window, xgwa.your_event_mask);
    }

    XSync (display, False);
}

NativeWindow::~NativeWindow()
{
    if( BaseWindow::standalone == mode )
    {
        XUnmapWindow(display, window);
        XDestroyWindow(display, window);
    }
    XSync(display, False);
    XCloseDisplay(display);
}

bool NativeWindow::process_events()
{
    while( XPending(display) )
    {
        XEvent event;
        XNextEvent (display, &event);

        if (event.xany.window == window)
        switch (event.xany.type)
        {
            case KeyPress:
            {
                KeySym keysym;
                char c = 0;
                XLookupString (&event.xkey, &c, 1, &keysym, 0);
                if (c == 3 // ^C
                 || c == 27// ESC
                ) return false;  // exit
                else if (! (keysym >= XK_Shift_L && keysym <= XK_Hyper_R))
                    XBell (display, 0);  // beep for non-chord keys
            }
            break;
            case ButtonPress:
                XBell (display, 0);
            break;
            case ConfigureNotify:
                resize(event.xconfigure.width, event.xconfigure.height);
            break;
            case ClientMessage:
            {
                if (event.xclient.message_type != XA_WM_PROTOCOLS)
                {
                    const char *s = XGetAtomName(display, event.xclient.message_type);
                    if (!s) s = "(null)";
                    fprintf (stderr, "unknown ClientMessage %s received!\n", s);
                }
                else if (event.xclient.data.l[0] != int (XA_WM_DELETE_WINDOW))
                {
                    const char *s1 = XGetAtomName(display, event.xclient.message_type);
                    const char *s2 = XGetAtomName(display, event.xclient.data.l[0]);
                    if (!s1) s1 = "(null)";
                    if (!s2) s2 = "(null)";
                    fprintf (stderr, "unknown ClientMessage %s[%s] received!\n",  s1, s2);
                }
                else return false;
            }
            break;
        }
    }
    return true;
}

void NativeWindow::get_size(unsigned int* width, unsigned int* height)const
{
    XWindowAttributes xgwa;
    XGetWindowAttributes (display, window, &xgwa);
    *width = xgwa.width;
    *height = xgwa.height;
}

Bool NativeWindow::map_notify_event (Display *dpy, XEvent *event, XPointer window)
{
    return event->xany.type == MapNotify && event->xvisibility.window == (Window) window;
}

int NativeWindow::x_error_handler(Display *dpy, XErrorEvent *error)
{
    char buffer[BUFSIZ];
    char mesg[BUFSIZ];
    const char* mtype = "XlibMessage";
    XGetErrorText(dpy, error->error_code, buffer, BUFSIZ);
    XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
    fprintf(stderr, "%s:  %s\n  ", mesg, buffer);
    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code %d", mesg, BUFSIZ);
    fprintf(stderr, mesg, error->request_code);
    return 0;
}

Window NativeWindow::root_window(Screen *screen)
{
    static Screen *save_screen = NULL;
    static Window root = 0;

    if (screen != save_screen)
    {
        Display *dpy = DisplayOfScreen(screen);

        const char *xss_id = getenv("XSCREENSAVER_WINDOW");
        if (xss_id && *xss_id)
        {
            unsigned long id = 0;
            char c = 0;
            if (1 == sscanf (xss_id, " 0x%lx %c", &id, &c) || 1 == sscanf (xss_id, " %lu %c",   &id, &c))
            {
                root = (Window) id;
                save_screen = screen;

                return root;
            }
        }

        root = RootWindowOfScreen(screen);

        // go look for a virtual root
        Atom __SWM_VROOT = XInternAtom(dpy, "__SWM_VROOT", False);

        Window rootReturn, parentReturn, *children;
        unsigned int numChildren;
        if (XQueryTree(dpy, root, &rootReturn, &parentReturn, &children, &numChildren))
        {
            for (unsigned int i = 0; i < numChildren; i++)
            {
                Atom actual_type;
                int actual_format;
                unsigned long nitems, bytesafter;
                unsigned char *newRoot = 0;

                if (XGetWindowProperty(dpy, children[i],__SWM_VROOT, 0, 1, False, XA_WINDOW,
                    &actual_type, &actual_format, &nitems, &bytesafter, &newRoot) == Success && newRoot)
                {
                    root = *((Window *) newRoot);
                    break;
                }
            }
            if (children)
                XFree((char *)children);
        }
        save_screen = screen;
    }
    return root;
}
//-----------------------------------------------------------------------------
