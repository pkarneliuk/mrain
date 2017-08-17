//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include <stdexcept>

#include "app_window.h"
#include "application.h"
#include "gl_renderer.h"
#include "options.h"
//------------------------------------------------------------------------------
const char AppWindow::caption[] = "Matrix Rain";
//------------------------------------------------------------------------------
AppWindow::AppWindow(Application* app, const Options& options)
: NativeWindow()
, application(app)
{
    if(options[Options::scrsvr_mode])
    {
        mode = AppWindow::screensaver;
        NativeWindow::create_window(this, 0, 0, 0);
    }
    else if(int id = options[Options::window_id])
    {
        mode = AppWindow::preview;
        NativeWindow::create_window(this, id, 0, 0);
    }
    else
    {
        mode             = AppWindow::standalone;
        const int width  = options[Options::width];
        const int height = options[Options::height];
        NativeWindow::create_window(this, 0, width, height);
    }
}

void AppWindow::repaint() { application->do_frame(); }
//-----------------------------------------------------------------------------
