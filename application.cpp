//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   application.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <csignal>

#include "application.h"
#include "app_window.h"
#include "capture.h"
#include "scene.h"
//-----------------------------------------------------------------------------
Application* Application::sig_handler = NULL;

Application::Application(const Options& opts):options(opts),window(NULL),capture(NULL),running(true)
{
    if( options[Options::scrsvr_mode] )
    {
        window = new AppWindow();
    }
    else if( int id = options[Options::window_id] )
    {
        window = new AppWindow(id);
    }
    else
    {
        int width  = options[Options::width];
        int height = options[Options::height];
        window = new AppWindow(width, height);
    }

    try
    {
        capture = new Capture(640, 480, options[Options::device]);
    }
    catch(runtime_error& error)
    {
        fprintf(stderr, "capture error: %s\n", error.what() );
    }

    Application::sig_handler = this;

    #ifdef UNIX
    struct sigaction action;
    action.sa_handler = &handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGTERM, &action, NULL);
    #endif//UNIX
}

Application::~Application()
{
    delete capture;
    delete window;
}

int Application::run()
{
    FPS counter(60);

    GLRenderer* renderer = window->get_renderer();

    BaseCapture::Buffer frame(capture, Capture::GRAY);

    Scene scene(renderer, capture != NULL ? &frame : NULL, options);

    while( running && window->process_events() )
    {
        unsigned long tick = counter.count_frame();

   //     frame.dump("tmp.bmp");
        scene.draw();
        scene.tick(tick);
        scene.present();
    }

    return 0;
}
//-----------------------------------------------------------------------------
