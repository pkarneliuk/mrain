//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   base_capture.cpp
// copyright:   (C) 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "base_capture.h"
#include "capture.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace {
    struct RGB { unsigned char r,g,b; };
    struct BGR { unsigned char b,g,r; };

    void GREYtoGREY(const unsigned char* src,
                    const unsigned char* end,
                          unsigned char* dst)
    {
        memcpy(dst, src, end - src);
    }

    template <typename Pixel>
    void YUY2toPixel(const unsigned char* src,
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

            Pixel& p1 = (Pixel&)*dst;

            p1.r = r;
            p1.g = g;
            p1.b = b;

            dst +=3;

            r = y2 + cr;
            b = y2 + cb;
            g = y2 - cg;
            CLAMP(r);
            CLAMP(g);
            CLAMP(b);

            Pixel& p2 = (Pixel&)*dst;

            p2.r = r;
            p2.g = g;
            p2.b = b;

            dst +=3;
        }

        #undef CLAMP
    }

    const auto YUY2toRGB = YUY2toPixel<RGB>;
    const auto YUY2toBGR = YUY2toPixel<BGR>;

    void YUY2toGRAY(const unsigned char* src,
                    const unsigned char* end,
                          unsigned char* dst)
    {
        for(; src<end; src+=4)
        {
            *dst++ = src[0];
            *dst++ = src[2];
        }
    }

    template <typename Pixel>
    void UYVYtoPixel(const unsigned char* src,
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

            Pixel& p1 = (Pixel&)*dst;

            p1.r = float(y1) + r;
            p1.g = float(y1) + g;
            p1.b = float(y1) + b;

            dst +=3;

            Pixel& p2 = (Pixel&)*dst;

            p2.r = float(y2) + r;
            p2.g = float(y2) + g;
            p2.b = float(y2) + b;

            dst +=3;
        }
    }

    const auto UYVYtoRGB = UYVYtoPixel<RGB>;
    const auto UYVYtoBGR = UYVYtoPixel<BGR>;

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

const BaseCapture::Transform BaseCapture::encoders[]=
{   //  FOURCC       bpp      RGB        BGR           GRAY
    { Capture::GREY, 8,  { 0,         0,            GREYtoGREY } }, // 8 greyscale
    { Capture::YUY2, 16, { YUY2toRGB, YUY2toBGR,    YUY2toGRAY } },
    { Capture::YUYV, 16, { YUY2toRGB, YUY2toBGR,    YUY2toGRAY } },
    { Capture::UYVY, 16, { UYVYtoRGB, UYVYtoBGR,    UYVYtoGRAY } },
    { Capture::P422, 8,  { 0,         0,         YUV422PtoGREY } },
    { 0            , 0,  { 0,         0,                     0 } }, // end of list
};

const BaseCapture::Transform& BaseCapture::get_transform(unsigned int fourcc)
{
    const Transform* p = &(encoders[0]);
    do
    {
        if(fourcc == p->fourcc) break;
        p++;
    }
    while(p->fourcc != 0);

    return *p;
}
//-----------------------------------------------------------------------------