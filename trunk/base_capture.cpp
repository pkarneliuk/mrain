//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   base_capture.cpp
// copyright:   (C) 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "base_capture.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace {
    void GREYtoGREY(const unsigned char* src,
                    const unsigned char* end,
                          unsigned char* dst)
    {
        memcpy(dst, src, end - src);
    }

    void YUYVtoRGB(const unsigned char* src,
                   const unsigned char* end,
                         unsigned char* dst)
    {
        #define CLAMP(c) if(c & (~255)) {c = (unsigned char)~(c >> (sizeof(c)*8 -1 ));}

        int r, g, b, cr, cg, cb, y1, y2;
        for(; src<end; src+=4)
        {
            y1 = src[0];
            cb = ((src[1] -128)*454) >> 8;
            cg = ((src[1] -128)*88);

            y2 = src[2];
            cr = ((src[3] - 128) * 359) >> 8;
            cg = (cg + (src[3] - 128) * 183) >> 8;

            r = y1 + cr;
            b = y1 + cb;
            g = y1 - cg;
            CLAMP(r);
            CLAMP(g);
            CLAMP(b);

            dst[0] = r;
            dst[1] = g;
            dst[2] = b;
            dst +=3;

            r = y2 + cr;
            b = y2 + cb;
            g = y2 - cg;
            CLAMP(r);
            CLAMP(g);
            CLAMP(b);

            dst[0] = r;
            dst[1] = g;
            dst[2] = b;
            dst +=3;
        }

        #undef CLAMP
    }

    void YUYVtoBGR(const unsigned char* src,
                   const unsigned char* end,
                         unsigned char* dst)
    {
        #define CLAMP(c) if(c & (~255)) {c = (unsigned char)~(c >> (sizeof(c)*8 -1 ));}

        int r, g, b, cr, cg, cb, y1, y2;
        for(; src<end; src+=4)
        {
            y1 = src[0];
            cb = ((src[1] -128)*454) >> 8;
            cg = ((src[1] -128)*88);

            y2 = src[2];
            cr = ((src[3] - 128) * 359) >> 8;
            cg = (cg + (src[3] - 128) * 183) >> 8;

            r = y1 + cr;
            b = y1 + cb;
            g = y1 - cg;
            CLAMP(r);
            CLAMP(g);
            CLAMP(b);

            dst[0] = b;
            dst[1] = g;
            dst[2] = r;
            dst +=3;

            r = y2 + cr;
            b = y2 + cb;
            g = y2 - cg;
            CLAMP(r);
            CLAMP(g);
            CLAMP(b);

            dst[0] = b;
            dst[1] = g;
            dst[2] = r;
            dst +=3;
        }

        #undef CLAMP
    }

    void YUYVtoGRAY(const unsigned char* src,
                    const unsigned char* end,
                          unsigned char* dst)
    {
        for(; src<end; src+=4)
        {
            *dst++ = src[0];
            *dst++ = src[2];
        }
    }

    void UYVYtoRGB(const unsigned char* src,
                   const unsigned char* end,
                         unsigned char* dst)
    {
        for(; src<end; src+=4) 
        {
            char u  = src[0];
            char y1 = src[1];
            char v  = src[2];
            char y2 = src[3];

            float r = + 2.032f*float(u);
            float g = - 0.395f*float(u) -0.581f*float(v);
            float b = + 1.140f*float(v);

            dst[0] = float(y1) + r;
            dst[1] = float(y1) + g;
            dst[2] = float(y1) + b;

            dst +=3;

            dst[0] = float(y2) + r;
            dst[1] = float(y2) + g;
            dst[2] = float(y2) + b;

            dst +=3;
        }
    }

    void UYVYtoBGR(const unsigned char* src,
                   const unsigned char* end,
                         unsigned char* dst)
    {
        for(; src<end; src+=4)
        {
            char u  = src[0];
            char y1 = src[1];
            char v  = src[2];
            char y2 = src[3];

            float r = + 2.032f*float(u);
            float g = - 0.395f*float(u) -0.581f*float(v);
            float b = + 1.140f*float(v);

            dst[0] = float(y1) + b;
            dst[1] = float(y1) + g;
            dst[2] = float(y1) + r;

            dst +=3;

            dst[0] = float(y2) + b;
            dst[1] = float(y2) + g;
            dst[2] = float(y2) + r;

            dst +=3;
        }
    }

    void UYVYtoGRAY(const unsigned char* src,
                    const unsigned char* end,
                          unsigned char* dst)
    {
        for(; src<end; src+=4)
        {
            *dst++ = src[1];
            *dst++ = src[3];
        }
    }

    void YUV422PtoGREY(const unsigned char* src,
                       const unsigned char* end,
                             unsigned char* dst)
    {
        memcpy(dst, src, end - src);
    }
}

const BaseCapture::Conversion BaseCapture::encoders[]=
{   //                     RGB        BGR           GRAY
    { FOURCC('GREY'), { 0,         0,            GREYtoGREY } }, // 8 greyscale
    { FOURCC('YUYV'), { YUYVtoRGB, YUYVtoBGR,    YUYVtoGRAY } },
    { FOURCC('UYVY'), { UYVYtoRGB, UYVYtoBGR,    UYVYtoGRAY } },
    { FOURCC('422P'), { 0,         0,         YUV422PtoGREY } },
    { FOURCC('YUY2'), { 0,         0,            YUYVtoGRAY } },
    { 0             , { 0,         0,                     0 } }, // end of list
};

BaseCapture::encoder BaseCapture::get_encoder(unsigned int fourcc, out_format fmt)
{
    for(size_t i=0; encoders[i].fourcc; i++)
    {
        if(fourcc == encoders[i].fourcc)
        {
            return encoders[i].func[fmt];
        }
    }
    return NULL;
}

bool BaseCapture::is_supported(unsigned int fourcc)
{
    for(size_t i=0; encoders[i].fourcc; i++)
    {
        if(fourcc == encoders[i].fourcc)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------