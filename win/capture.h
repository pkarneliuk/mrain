//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include <Mfapi.h>
#include <Mfidl.h>
#include <Mfreadwrite.h>
#include <atlbase.h>
#include <windows.h>

#include "base_capture.h"
//------------------------------------------------------------------------------
class Capture : public BaseCapture
{
    class CriticalSection
    {
    public:
        class Lock
        {
        public:
            Lock(CriticalSection& c)
            : cs(c)
            {
                EnterCriticalSection(&cs.cs);
            }
            ~Lock() { LeaveCriticalSection(&cs.cs); }

        private:
            Lock(Lock&);          // undefined
            void operator=(Lock&);// undefined

            CriticalSection& cs;
        };

        CriticalSection() { InitializeCriticalSection(&cs); }
        ~CriticalSection() { DeleteCriticalSection(&cs); }

    private:
        CriticalSection(CriticalSection&);// undefined
        void operator=(CriticalSection&); // undefined

        CRITICAL_SECTION cs;
    };

public:
    enum Native4CC
    {
        GREY = FCC('GREY'),
        YUYV = FCC('YUYV'),
        UYVY = FCC('UYVY'),
        P422 = FCC('422P'),
        YUY2 = FCC('YUY2'),
    };

    Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name);
    ~Capture();

    virtual bool set_buffer(unsigned char* buf, out_format fmt);
    HRESULT async_read() const;

    void decode_to_buffer(unsigned char* src, unsigned int length) const;
    void decode_padded_to_buffer(unsigned char* scanline0, long pitch) const;

    static unsigned int enum_devices(char               buffers[][128],
                                     const unsigned int size) throw();

private:
    static IMFActivate* find_device(const char* dev_name);

    CComPtr<IMFSourceReader> reader;
    INT32                    stride;

    mutable CriticalSection cs;
};
//------------------------------------------------------------------------------
