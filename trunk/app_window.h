//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   app_window.h
// copyright:   (C) 2008, 2009, 2014 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef APP_WINDOW_H
#define APP_WINDOW_H
//-----------------------------------------------------------------------------
#include "gl_renderer.h"
#include "native_window.h"
//-----------------------------------------------------------------------------
class AppWindow : private NativeWindow
{
public:
    static const char caption[];
    enum Mode{ preview, standalone, screensaver };

    AppWindow(class Application* app, const Options& options);
    ~AppWindow();

    void activate      () { NativeWindow::activate();              }
    bool process_events() { return NativeWindow::process_events(); }

    inline GLRenderer* get_renderer(){ return renderer; }

private:
    friend class NativeWindow;

    void resize(unsigned int width, unsigned int height)
    {
        renderer->reshape(width, height);
    }

    void create_renderer()
    {
        renderer = new GLRenderer(this);
    }

    void destroy_renderer()
    {
        delete renderer;
    }

    void repaint();

    class Application* application;
    class GLRenderer*  renderer;
    Mode mode;
};
//-----------------------------------------------------------------------------
#endif//APP_WINDOW_H
//-----------------------------------------------------------------------------
