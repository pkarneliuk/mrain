//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_window.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef NATIVE_WINDOW_H
#define NATIVE_WINDOW_H
//-----------------------------------------------------------------------------
#include <X11/Xlib.h>

#include "base_window.h"
#include "options.h"
//-----------------------------------------------------------------------------
class NativeWindow:protected BaseWindow
{
    friend class GLContext;
public:
    NativeWindow(BaseWindow::Mode mode, unsigned int id, int width, int height);
    ~NativeWindow();

    bool process_events();
    void get_size(unsigned int* width, unsigned int* height)const;

protected:
    Display* display;
    Window window;

private:
    Atom XA_WM_PROTOCOLS;
    Atom XA_WM_DELETE_WINDOW;

    static Bool map_notify_event (Display *dpy, XEvent *event, XPointer window);
    static int x_error_handler(Display *dpy, XErrorEvent *error);
    static Window root_window(Screen *screen);
};

class Config
{
public:
    Config(Options& opts):options(opts)
    {
    }

    Config& operator=(Config&);  // undefined

    bool show_dialog(int parent_id)
    {
        return true;
    }

private:
    Options&  options;
};
//-----------------------------------------------------------------------------
#endif//NATIVE_WINDOW_H
//-----------------------------------------------------------------------------
