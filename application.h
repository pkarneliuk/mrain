//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   application.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
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

private:

    class FPS
    {
    public:
        FPS();                  //undefined
        FPS& operator=(FPS&);   //undefined

        FPS(unsigned int limit):lim((limit == 0) ? 0 : (1000000/limit)),the_sec(0),fps(0),curr_fps(0){}

        unsigned long count_frame()
        {
            curr_fps++;
            unsigned long compute_time = timer.tick();
            if(compute_time < lim)
            {
                sleeep(lim - compute_time);
            }
            unsigned long frame_time = compute_time + timer.tick();
            the_sec += frame_time;
            if( the_sec > 1000000 )
            {
                the_sec -= 1000000;
                fps = curr_fps; // save count per frame
                curr_fps = 0;
            //    printf("FPS: %i\n", fps);
            }
            return frame_time;
        }
        
        operator unsigned int()const{ return fps; }
    private:
        Timer timer;
        const unsigned int lim;
        unsigned int the_sec;
        unsigned int fps;
        unsigned int curr_fps;
    };

    inline void signal(int){ running = false; }
    static void handler(int sig)
    {
        if(sig_handler) sig_handler->signal(sig);
    }

    static Application* sig_handler;

    const Options&      options;
    class AppWindow*    window;
    class Capture*      capture;
    bool running;
};
//-----------------------------------------------------------------------------
#endif//APPLICATION_H
//-----------------------------------------------------------------------------
