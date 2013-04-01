//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   base_capture.h
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef BASE_CAPTURE_H
#define BASE_CAPTURE_H
//-----------------------------------------------------------------------------
#include "bitmap.h"
//-----------------------------------------------------------------------------

#define FOURCC(ch4) ((((unsigned int)(ch4) & 0x000000FF) << 24) | \
                     (((unsigned int)(ch4) & 0x0000FF00) << 8)  | \
                     (((unsigned int)(ch4) & 0x00FF0000) >> 8)  | \
                     (((unsigned int)(ch4) & 0xFF000000) >> 24))

class BaseCapture
{
public:
    enum out_format{ RGB, BGR, GRAY, num };

    static inline size_t bpp(out_format format) // bytes per pixel
    {
        switch(format)
        {
        case RGB:
        case BGR:
            return 3;
        case GRAY:
            return 1;
        default:;
        }
        return 0;
    }

    BaseCapture():fourcc(0),w(0),h(0),format(num),buffer(NULL),coder(NULL){}

    unsigned int width () { return w; }
    unsigned int height() { return h; }

    virtual bool set_buffer(unsigned char* buf, out_format fmt)
    {
        format = fmt;
        buffer = buf;
        return true;
    }

    class Buffer: public Bitmap
    {
    public:
        Buffer(class BaseCapture* c, const BaseCapture::out_format format):capture(c)
        {
            if (capture != NULL)
            {
                Bitmap::image_width = capture->width();
                Bitmap::image_height = capture->height();

                Bitmap::clear();

                size_t size = image_width*image_height*BaseCapture::bpp(format);
                Bitmap::buffer = new unsigned char [size];

                capture->set_buffer(buffer, format);
            }
        }

        ~Buffer()
        {
            if (capture != NULL)
            {
                // reset capture
                capture->set_buffer(NULL, num);
            }
        }

    private:
        BaseCapture* capture;
    };

protected:

    typedef void (*encoder)(const unsigned char* src, const unsigned char* end, unsigned char* dst);

    const static struct Conversion
    {
        unsigned int fourcc;
        encoder func[num];
    } encoders[];

    static encoder get_encoder(unsigned int fourcc, out_format fmt);
    static bool   is_supported(unsigned int fourcc);


    unsigned int fourcc;    // FOURCC of video encoding
    unsigned int w;         // resolutions of video frame
    unsigned int h;

    out_format format;      // output format
    unsigned char* buffer;  // output buffer
    encoder coder;
};
//-----------------------------------------------------------------------------
#endif//BASE_CAPTURE_H
//-----------------------------------------------------------------------------
