//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "options.h"
#include "scene.h"
#include "stuff.h"
#include <cstdlib>
#include <memory>
//------------------------------------------------------------------------------
class AppWindow;
class Capture;
class Scene;

class Application : noncopyable
{
public:
    explicit Application(const Options& opts);

    int  run();
    void do_frame();

private:
    static void signal_handler(int sig);

    const Options&             options;
    std::unique_ptr<AppWindow> window;
    std::unique_ptr<Capture>   capture;
    std::unique_ptr<Scene>     scene;
    FPS                        fps;
    bool                       running;
};
//------------------------------------------------------------------------------
