//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   application.h
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef APPLICATION_H
#define APPLICATION_H
//-----------------------------------------------------------------------------
#include <cstdlib>
#include <memory>

#include "options.h"
#include "scene.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
class AppWindow;
class Capture;
class Scene;

class Application : noncopyable
{
public:
    explicit Application(const Options& opts);

    int run();
    void do_frame();

private:
    static void signal_handler(int sig);

    const Options&             options;
    std::unique_ptr<AppWindow> window;
    std::unique_ptr<Capture>   capture;
    std::unique_ptr<Scene>     scene;
    FPS                        fps;
    bool running;
};
//-----------------------------------------------------------------------------
#endif//APPLICATION_H
//-----------------------------------------------------------------------------
