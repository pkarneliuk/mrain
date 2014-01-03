//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   application.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <csignal>
#include <iostream>

#include "application.h"
#include "app_window.h"
#include "capture.h"
#include "scene.h"
//-----------------------------------------------------------------------------
static Application* instance = NULL;

void Application::signal_handler(int sig)
{
    if(instance) instance->running = false;
}

Application::Application(const Options& opts)
    : options(opts)
    , window (NULL)
    , capture(NULL)
    , scene  (NULL)
    , fps    (60)
    , running(true)
{

    window = new AppWindow(this, options);

    try
    {
        capture = new Capture(640, 480, options[Options::device]);
    }
    catch(runtime_error& error)
    {
        std::cerr << "capture error: " << error.what() << std::endl;
    }

    instance = this;

    #ifdef UNIX
    struct sigaction action;
    action.sa_handler = &Application::signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGTERM, &action, NULL);
    #endif//UNIX

    scene = new Scene(window->get_renderer(), capture, options);
}

Application::~Application()
{
    delete scene;
    delete capture;
    delete window;
}

int Application::run()
{
    window->activate();

    while( running && window->process_events() )
    {
        do_frame();
    }

    return 0;
}

void Application::do_frame()
{
    const unsigned long tick = fps.count_frame();

    scene->draw();
    scene->tick(tick);
    scene->present();
}
//-----------------------------------------------------------------------------
