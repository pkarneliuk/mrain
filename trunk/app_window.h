//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   app_window.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef APP_WINDOW_H
#define APP_WINDOW_H
//-----------------------------------------------------------------------------
#include "native_window.h"
//-----------------------------------------------------------------------------
class AppWindow : private NativeWindow
{
public:
    AppWindow(class Application* app);
    AppWindow(class Application* app, unsigned int parent_id);
    AppWindow(class Application* app, int width, int height);
    ~AppWindow();

    bool process_events();

    inline GLRenderer* get_renderer(){ return renderer; }

protected:
    virtual void repaint();

private:
    class Application* application;
};
//-----------------------------------------------------------------------------
#endif//APP_WINDOW_H
//-----------------------------------------------------------------------------
