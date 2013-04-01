//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   stuff.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef STUFF_H
#define STUFF_H
//-----------------------------------------------------------------------------
#include <stdexcept>
#include <cstdio>
#include <cstdarg>

#include "native_stuff.h"
//-----------------------------------------------------------------------------
class runtime_error:public std::exception
{
public:
    runtime_error(const char* format, ...)throw()
    {
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
    }

    const char* what()const throw(){ return buffer; }
private:
    char buffer[1024];
};

class Random
{
    enum { table_size=55 };
public:
    enum { random_max=0xFFFFFFFFL };
    Random(unsigned int seed=0);


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
    explicit Counter(unsigned int lim):limit(lim), count(0U){}

    inline unsigned int test(unsigned int delta)
    {   // integer calculations !
        // Worning ! There is an overflow !
        count += delta;
        unsigned int uptimes = count/limit;
        count %= limit;
        return uptimes;
    }
private:
    unsigned int limit;
    unsigned int count;
};

class Version
{
public:
    Version(const char* string);
    Version(unsigned char major=0, unsigned char minor=0, unsigned char release=0);
    Version(const Version& v):iversion(v.iversion){}

    Version& operator=(const Version& v){ iversion = v.iversion; return *this; }
    Version& operator=(const char* str);

    inline bool operator>=(Version v)const{ return iversion >= v.iversion; }
    inline bool operator<=(Version v)const{ return iversion <= v.iversion; }
    inline bool operator==(Version v)const{ return iversion == v.iversion; }

    bool operator>=(const char* str);

private:
    unsigned int iversion; 
};

struct BMP_INFOHEADER
{
    unsigned int    biSize;
    unsigned long   biWidth;
    unsigned long   biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    unsigned long   biXPelsPerMeter;
    unsigned long   biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
};

unsigned int make_seed();

// put path to 'file_name' at user`s home folder into buffer
unsigned int inhome_path(char* buffer, size_t size, const char* file_name);

bool convert_bmp_2_include_array(char* bmp_file, char* array_name);

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
//-----------------------------------------------------------------------------
#endif//STUFF_H
//-----------------------------------------------------------------------------
