//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "native_stuff.h"
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <experimental/filesystem>
#include <memory>
#include <stdexcept>
#include <thread>
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

    static unsigned int make_seed();

private:
    int index1, index2;

    static unsigned int table[table_size];
};

extern Random grandom;

class Counter
{
public:
    explicit Counter(std::size_t lim)
    : limit{lim}
    {
    }

    std::size_t test(std::size_t delta)
    {// integer calculations!
        // Warning! There is an overflow!
        count += delta;
        std::size_t uptimes = count / limit;
        count %= limit;
        return uptimes;
    }

private:
    std::size_t limit;
    std::size_t count = 0;
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

class Timer
{
    friend class Waiter;

public:
    using clock = std::chrono::high_resolution_clock;

    Timer() { prev = begin = clock::now(); }

    std::chrono::microseconds time()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
            clock::now() - begin);
    }
    std::chrono::microseconds tick()
    {
        auto curr = clock::now();
        auto t    = curr - prev;
        prev      = curr;
        return std::chrono::duration_cast<std::chrono::microseconds>(t);
    }

private:
    clock::time_point prev;
    clock::time_point begin;
};

class Waiter
{
    static Timer timer;

public:
    explicit Waiter(std::chrono::microseconds microseconds)
    : end{timer.prev + microseconds}
    {
    }

    bool        ready() const { return (end < timer.prev); }
    static void tick() { timer.tick(); }

private:
    const Timer::clock::time_point end;
};

class FPS : noncopyable
{
public:
    FPS(unsigned int limit)
    : lim((limit == 0) ? 0 : (1'000'000 / limit))
    {
    }

    Timer::clock::rep count_frame()
    {
        cfps++;
        const auto compute_time = timer.tick();
        if(compute_time < lim)
        {
            std::this_thread::sleep_for(lim - compute_time);
        }
        auto frame_time = compute_time + timer.tick();
        the_sec += frame_time;
        if(the_sec > std::chrono::microseconds{1'000'000})
        {
            the_sec -= std::chrono::microseconds{1'000'000};
            fps  = cfps;// save count per frame
            cfps = 0;
            // printf("FPS: %zu\n", fps);
        }
        return frame_time.count();
    }

    operator std::size_t() const noexcept { return fps; }

private:
    Timer       timer;
    std::size_t fps  = 0;
    std::size_t cfps = 0;

    const std::chrono::microseconds lim;
    std::chrono::microseconds       the_sec{0};
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

// put path to 'file_name' at user`s home folder into buffer
namespace fs = std::experimental::filesystem;
fs::path inhome_path(const fs::path& file_name);

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
