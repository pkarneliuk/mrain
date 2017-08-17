//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "application.h"
#include "app_window.h"
#include "capture.h"
#include "scene.h"
#include <csignal>
#include <iostream>
//------------------------------------------------------------------------------
static Application* instance = nullptr;

void Application::signal_handler(int)
{
    if(instance)
        instance->running = false;
}

Application::Application(const Options& opts)
: options(opts)
, fps(60)
, running(true)
{
    window = std::make_unique<AppWindow>(this, options);

    try
    {
        capture = std::make_unique<Capture>(640, 480, options[Options::device]);
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
#endif// UNIX

    GLRenderer* renderer = window->get_renderer();
    scene = std::make_unique<Scene>(renderer, capture.get(), options);
}

int Application::run()
{
    window->activate();

    while(running && window->process_events()) {
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
//------------------------------------------------------------------------------
