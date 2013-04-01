//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   capture.h
// copyright:   (C) 2008, 2009, 2012, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef CAPTURE_H
#define CAPTURE_H
//-----------------------------------------------------------------------------
#include <windows.h>
#include <atlbase.h>
#include <Mfapi.h>
#include <Mfidl.h>
#include <Mfreadwrite.h>

#include "base_capture.h"
//-----------------------------------------------------------------------------
class Capture:public BaseCapture
{
    class CriticalSection
    {
    public:
        class Lock
        {
        public:
            Lock(CriticalSection& c):cs(c) { EnterCriticalSection(&cs.cs); }
            ~Lock()                        { LeaveCriticalSection(&cs.cs); }
        private:
            Lock(Lock&);            // undefined
            void operator=(Lock&);  // undefined

            CriticalSection& cs;
        };

        CriticalSection()  { InitializeCriticalSection(&cs); }
        ~CriticalSection() { DeleteCriticalSection(&cs);     }
    private:
        CriticalSection(CriticalSection&);  // undefined
        void operator=(CriticalSection&);   // undefined

        CRITICAL_SECTION cs;
    };

public:

    enum Native4CC {
        GREY = FCC('GREY'),
        YUYV = FCC('YUYV'),
        UYVY = FCC('UYVY'),
        P422 = FCC('422P'),
        YUY2 = FCC('YUY2'),
    };

    Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name);
    ~Capture();

    virtual bool set_buffer(unsigned char* buf, out_format fmt);

    static unsigned int enum_devices(char buffers[][128], const unsigned int size)throw();

    void decode_to_buffer(unsigned char* src, unsigned int length);
    void decode_padded_to_buffer(unsigned char* scanline0, unsigned int pitch);
private:

    static IMFActivate* find_device(const char* dev_name);

    friend class Sampler;

    CComPtr<IMFSourceReader> reader;
    INT32 stride;

    CriticalSection cs;
};
//-----------------------------------------------------------------------------
#endif//CAPTURE_H
//-----------------------------------------------------------------------------
