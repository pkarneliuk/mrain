//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   app_window.cpp
// copyright:   (C) 2008, 2009, 2014 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <stdexcept>

#include "application.h"
#include "app_window.h"
#include "gl_renderer.h"
#include "options.h"
//-----------------------------------------------------------------------------
const char AppWindow::caption[] = "Matrix Rain";
//-----------------------------------------------------------------------------
AppWindow::AppWindow(Application* app, const Options& options)
    : NativeWindow()
    , application(app)
    , renderer(NULL)
{
    if( options[Options::scrsvr_mode] )
    {
        mode = AppWindow::screensaver;
        NativeWindow::create_window(this, 0, 0, 0);
    }
    else if( int id = options[Options::window_id] )
    {
        mode = AppWindow::preview;
        NativeWindow::create_window(this, id, 0, 0);
    }
    else
    {
        mode = AppWindow::standalone;
        const int width  = options[Options::width];
        const int height = options[Options::height];
        NativeWindow::create_window(this, 0, width, height);
    }
}

AppWindow::~AppWindow()
{
}

void AppWindow::repaint()
{
    application->do_frame();
}
//-----------------------------------------------------------------------------
