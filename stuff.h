//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "native_stuff.h"
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <stdexcept>
//------------------------------------------------------------------------------
class runtime_error : public std::exception
{
public:
    runtime_error(const char* format, ...) throw()
    {
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
    }

    const char* what() const noexcept { return buffer; }

private:
    char buffer[1024];
};

struct noncopyable
{
    noncopyable()                   = default;
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
};

class Random
{
    enum
    {
        table_size = 55
    };

public:
    enum
    {
        random_max = 0xFFFFFFFFL
    };
    Random(unsigned int seed = 0);

    unsigned int operator()();
    float operator()(float f);
    unsigned int operator()(unsigned int i);

private:
    int index1, index2;

    static unsigned int table[table_size];
};

extern Random grandom;

class Counter
{
public:
    explicit Counter(unsigned int lim)
    : limit{lim}
    {
    }

    inline unsigned int test(unsigned int delta)
    {// integer calculations!
        // Warning! There is an overflow!
        count += delta;
        unsigned int uptimes = count / limit;
        count %= limit;
        return uptimes;
    }

private:
    unsigned int limit;
    unsigned int count = 0;
};

class Version
{
public:
    explicit Version(const char* string);
    Version(std::uint8_t maj = 0, std::uint8_t min = 0, std::uint8_t r = 0);
    Version(const Version&) = default;
    Version& operator=(const Version&) = default;
    Version& operator                  =(const char* str);

    inline bool operator>=(Version v) const { return iversion >= v.iversion; }
    inline bool operator<=(Version v) const { return iversion <= v.iversion; }
    inline bool operator==(Version v) const { return iversion == v.iversion; }

    bool operator>=(const char* str);

private:
    unsigned int iversion;
};

class FPS : noncopyable
{
public:
    FPS(unsigned int limit)
    : lim((limit == 0) ? 0 : (1000000 / limit))
    {
    }

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
        if(the_sec > 1000000)
        {
            the_sec -= 1000000;
            fps      = curr_fps;// save count per frame
            curr_fps = 0;
            //    printf("FPS: %i\n", fps);
        }
        return frame_time;
    }

    operator unsigned int() const { return fps; }

private:
    Timer              timer;
    const unsigned int lim;
    unsigned int       the_sec  = 0;
    unsigned int       fps      = 0;
    unsigned int       curr_fps = 0;
};

struct BMP_INFOHEADER
{
    unsigned int   biSize;
    unsigned long  biWidth;
    unsigned long  biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int   biCompression;
    unsigned int   biSizeImage;
    unsigned long  biXPelsPerMeter;
    unsigned long  biYPelsPerMeter;
    unsigned int   biClrUsed;
    unsigned int   biClrImportant;
};

unsigned int make_seed();

// put path to 'file_name' at user`s home folder into buffer
unsigned int inhome_path(char* buffer, size_t size, const char* file_name);

bool convert_bmp_2_include_gray_array(char* bmp_file, char* array_name);

inline unsigned int round_pow_2(unsigned int a)
{
    --a;
    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;
    return ++a;
}
//------------------------------------------------------------------------------
