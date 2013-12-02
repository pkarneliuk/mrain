//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   app_window.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdlib>
#include <cstring>
#include <string>
#include <stdexcept>

#include "gl_renderer.h"
#include "application.h"
#include "app_window.h"
#include "options.h"
//-----------------------------------------------------------------------------
AppWindow::AppWindow(Application* app)
    : NativeWindow(BaseWindow::screensaver, 0, 0, 0)
    , application(app)
{
}

AppWindow::AppWindow(Application* app, unsigned int parent_id)
    : NativeWindow(BaseWindow::preview, parent_id, 0, 0)
    , application(app)
{
}

AppWindow::AppWindow(Application* app, int width, int height)
    : NativeWindow(BaseWindow::standalone, 0, width, height)
    , application(app)
{
}

AppWindow::~AppWindow()
{
}

bool AppWindow::process_events()
{
    return NativeWindow::process_events();
}

void AppWindow::repaint()
{
    application->do_frame();
}
//-----------------------------------------------------------------------------
