//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_stuff.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdarg>
#include <locale>

#include "native_stuff.h"
//-----------------------------------------------------------------------------
Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    gettimeofday(&begin, NULL);
    prev = begin;
}

unsigned long Timer::time()
{
    struct timeval now;
    gettimeofday(&now, NULL);

    return (now.tv_sec - begin.tv_sec) * 1000000 - begin.tv_usec + now.tv_usec;
}

unsigned long Timer::tick()
{
    struct timeval now;
    gettimeofday(&now, NULL);

    unsigned long delta = (now.tv_sec - prev.tv_sec) * 1000000 - prev.tv_usec + now.tv_usec;
    prev = now;

    return delta;
}

//-----------------------------------------------------------------------------

Timer Waiter::timer;

Waiter::Waiter(unsigned long microseconds):sec(microseconds / 1000000), usec(microseconds % 1000000)
{
    end.tv_sec = timer.prev.tv_sec  + sec;
    end.tv_usec = timer.prev.tv_usec+usec;
}

bool Waiter::ready()
{
    bool done = false;
    if(end.tv_sec < timer.prev.tv_sec)
    {
        done = true;
    }
    else if(end.tv_sec == timer.prev.tv_sec)
    {
        done = (end.tv_usec <= timer.prev.tv_usec);
    }

    if( done )
    {
//      end.tv_sec = timer.prev.tv_sec  + sec;
//      end.tv_usec = timer.prev.tv_usec+usec;

        unsigned long u = end.tv_usec+usec;
        end.tv_usec= u % 1000000;
        end.tv_sec+= sec + u / 1000000;
    }
    return done;
}

void Waiter::tick()
{
    timer.tick();
}

//-----------------------------------------------------------------------------

unsigned int make_seed()
{
    struct timeval tp;
    #ifdef GETTIMEOFDAY_TWO_ARGS
        struct timezone tzp;
        gettimeofday(&tp, &tzp);
    #else
        gettimeofday(&tp);
    #endif
    return (999*tp.tv_sec) + (1001*tp.tv_usec) + (1003 * getpid());
}

unsigned int inhome_path(char* buffer, size_t size, const char* file_name)
{
    int len = snprintf(buffer, size, "~/%s", file_name);

    return len < int(size) ? len : 0;
}
//-----------------------------------------------------------------------------
