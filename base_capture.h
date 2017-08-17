//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "bitmap.h"
//------------------------------------------------------------------------------
class BaseCapture : noncopyable
{
public:
    enum out_format
    {
        RGB,
        BGR,
        GRAY,
        num,
    };

    static inline unsigned int bpp(out_format format)// bytes per pixel
    {
        switch(format)
        {
        case RGB:
        case BGR: return 3;
        case GRAY: return 1;
        default:;
        }
        return 0;
    }

    BaseCapture()          = default;
    virtual ~BaseCapture() = default;

    unsigned int width() { return w; }
    unsigned int height() { return h; }

    virtual bool set_buffer(unsigned char* buf, out_format fmt)
    {
        format = fmt;
        buffer = buf;
        return true;
    }

    class Buffer : public Bitmap
    {
    public:
        Buffer(class BaseCapture* c, const BaseCapture::out_format format)
        : capture(c)
        {
            if(capture != NULL)
            {
                Bitmap::image_width  = capture->width();
                Bitmap::image_height = capture->height();

                Bitmap::buffer.clear();

                size_t size =
                    image_width * image_height * BaseCapture::bpp(format);
                buffer.resize(size);

                capture->set_buffer(&(buffer[0]), format);
            }
        }

        ~Buffer()
        {
            if(capture != NULL)
            {
                // reset capture
                capture->set_buffer(NULL, num);
            }
        }

    private:
        BaseCapture* capture;
    };

protected:
    using encoder = void (*)(const unsigned char* src, const unsigned char* end,
                             unsigned char* dst);

    const static struct Transform
    {
        const unsigned int fourcc;
        const size_t       bpp;// bits per pixel
        const encoder      func[num];
    } encoders[];

    static const Transform& get_transform(unsigned int fourcc);
    static inline bool is_supported(unsigned int fourcc)
    {
        return get_transform(fourcc).fourcc != 0;
    }
    static inline size_t get_bpp(unsigned int fourcc)
    {
        return get_transform(fourcc).bpp;
    }

    unsigned int fourcc = 0;// FOURCC of video encoding
    unsigned int w      = 0;// resolutions of video frame
    unsigned int h      = 0;
    unsigned int rate   = 0;

    out_format     format = num;    // output format
    unsigned char* buffer = nullptr;// output buffer
    encoder        coder  = nullptr;
};
//------------------------------------------------------------------------------
