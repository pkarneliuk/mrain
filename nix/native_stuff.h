//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_stuff.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef NATIVE_STUFF_H
#define NATIVE_STUFF_H
//-----------------------------------------------------------------------------
#include <sys/time.h>
//-----------------------------------------------------------------------------
class Timer
{
    friend class Waiter;
public:
    Timer();

    void reset();
    unsigned long time();
    unsigned long tick();
private:
    struct timeval prev;
    struct timeval begin;
};

class Waiter
{
    static Timer timer;
public:
    Waiter(unsigned long microseconds);

    bool ready();
    static void tick();
private:
    unsigned long sec;
    unsigned long usec;

    struct timeval end;
};

struct __attribute__ ((__packed__)) BMP_FILEHEADER
{
    unsigned char   bfType[2];
    unsigned int    bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned int    bfOffBits;
};

inline void sleeep(unsigned long microseconds)
{
    usleep(microseconds);
}
//-----------------------------------------------------------------------------
#endif//NATIVE_STUFF_H
//-----------------------------------------------------------------------------
