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
        auto int width  = options[Options::width];
        auto int height = options[Options::height];
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
    FPS counter(30);

    GLRenderer* renderer = window->get_renderer();


    Scene scene(renderer, capture, options);

    while( running && window->process_events() )
    {
        unsigned long tick = counter.count_frame();

        if(capture)
        {
            capture->capture();
        }
        
        scene.draw();
        scene.tick(tick);
        scene.present();
    }

    return 0;
}

inline void Application::sleep(unsigned long microseconds)
{
    #ifdef UNIX
        usleep(microseconds);
    #endif//UNIX

    #ifdef WIN32
        Sleep(microseconds/1000);
    #endif//WIN32
}
//-----------------------------------------------------------------------------
