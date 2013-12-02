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

#include "options.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
class Application
{
public:
    Application(const Options& opts);
    ~Application();
    Application& operator=(Application&); //undefined

    int run();

    void do_frame();

private:
    static void signal_handler(int sig);

    const Options&      options;
    class AppWindow*    window;
    class Capture*      capture;
    class Scene*        scene;
    FPS                 fps;
    bool running;
};
//-----------------------------------------------------------------------------
#endif//APPLICATION_H
//-----------------------------------------------------------------------------
