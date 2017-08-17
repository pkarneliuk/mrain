//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "gl_renderer.h"
#include "native_window.h"
#include "stuff.h"
//------------------------------------------------------------------------------
class AppWindow : private NativeWindow
{
public:
    static const char caption[];
    enum Mode
    {
        preview,
        standalone,
        screensaver
    };

    AppWindow(class Application* app, const Options& options);

    void activate() { NativeWindow::activate(); }
    bool process_events() { return NativeWindow::process_events(); }

    inline GLRenderer* get_renderer() { return renderer.get(); }

private:
    friend class NativeWindow;

    void resize(unsigned int width, unsigned int height)
    {
        renderer->reshape(width, height);
    }

    void create_renderer() { renderer.reset(new GLRenderer(this)); }

    void destroy_renderer() { renderer.reset(); }

    void repaint();

    class Application*          application;
    std::unique_ptr<GLRenderer> renderer;
    Mode                        mode;
};
//------------------------------------------------------------------------------
