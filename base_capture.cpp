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

    struct YUY2 { unsigned char y0,u,y1,v; };
    struct UYVY { unsigned char u,y0,v,y1; };

    void GREYtoGREY(const unsigned char* src,
                    const unsigned char* end,
                          unsigned char* dst)
    {
        memcpy(dst, src, end - src);
    }

    template <typename YUV, typename Pixel>
    void YUV422toPixel(const unsigned char* src,
                       const unsigned char* end,
                             unsigned char* dst)
    {
        #define CLAMP(c) if(c & (~255)) {c = (unsigned char)~(c >> (sizeof(c)*8 -1 ));}

        int r, g, b, cr, cg, cb, y0, y1;
        for(; src<end; src+=4)
        {
            YUV& data = (YUV&)*src;

            y0 = data.y0;
            cb = ((data.u -128)*454) >> 8;
            cg = ((data.u -128)*88);

            y1 = data.y1;
            cr = (     (data.v - 128) * 359) >> 8;
            cg = (cg + (data.v - 128) * 183) >> 8;

            r = y0 + cr;
            b = y0 + cb;
            g = y0 - cg;
            CLAMP(r);
            CLAMP(g);
            CLAMP(b);

            Pixel& p1 = (Pixel&)*dst;

            p1.r = r;
            p1.g = g;
            p1.b = b;

            dst +=3;

            r = y1 + cr;
            b = y1 + cb;
            g = y1 - cg;
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

    template <typename YUV>
    void YUV422toGRAY(const unsigned char* src,
                      const unsigned char* end,
                            unsigned char* dst)
    {
        for(; src<end; src+=4)
        {
            YUV& data = (YUV&)*src;
            *dst++ = data.y0;
            *dst++ = data.y1;
        }
    }

    const auto YUY2toRGB  = YUV422toPixel<YUY2,RGB>;
    const auto YUY2toBGR  = YUV422toPixel<YUY2,BGR>;
    const auto YUY2toGRAY = YUV422toGRAY<YUY2>;

    const auto UYVYtoRGB  = YUV422toPixel<UYVY,RGB>;
    const auto UYVYtoBGR  = YUV422toPixel<UYVY,BGR>;
    const auto UYVYtoGRAY = YUV422toGRAY<UYVY>;

    void YUV422PtoGREY(const unsigned char* src,
                       const unsigned char* end,
                             unsigned char* dst)
    {
        memcpy(dst, src, end - src);
    }
}

const BaseCapture::Transform BaseCapture::encoders[]=
{   //  FOURCC       bpp      RGB        BGR           GRAY
    { Capture::GREY, 8,  { 0,         0,            GREYtoGREY } },
    { Capture::YUY2, 16, { YUY2toRGB, YUY2toBGR,    YUY2toGRAY } },
    { Capture::YUYV, 16, { YUY2toRGB, YUY2toBGR,    YUY2toGRAY } }, // duplicate of YUY2
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