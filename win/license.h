//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   license.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef LICENSE_H
#define LICENSE_H
//-----------------------------------------------------------------------------
#include <windows.h>
//-----------------------------------------------------------------------------
// License stuff

#define RKEY_SIZE 32
#define AKEY_SIZE 64

typedef void(*akey_check)(void* data);

extern char rkey[RKEY_SIZE];
extern char akey[AKEY_SIZE];



extern DWORD rdtsc_low; // not initialized

extern akey_check* checkers;
extern akey_check default_checkers[];
extern unsigned int checkers_count;

void create_license(void* data);

namespace License
{
    void mbid(char buf[128]);
    void osid(char buf[35]);
    void hdid(char buf[4]);
    void time(char buf[8]);

    void print_info();
    void print_syst(char buf[8]);

    struct RKey
    {
        char rkey[32];
    };
    
    class AKey
    {
    public:
        typedef void (AKey::*creator)();
        enum { key_strlen = 16 };
        explicit AKey(const char* hex64);


        /*
            64 bit key
            [    |    |    |    ]
            60 bit - data packed 10 chars/6 bit
            4  bit - meta
        */
    private:
        unsigned __int64 akey;
    };
};

inline void set_rdtsc_low()
{
    __asm rdtsc
    __asm mov rdtsc_low, eax
}
//-----------------------------------------------------------------------------
#endif//LICENSE_H
//-----------------------------------------------------------------------------
