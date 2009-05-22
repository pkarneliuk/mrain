//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   base_capture.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef BASE_CAPTURE_H
#define BASE_CAPTURE_H
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class BaseCapture
{
public:
    enum out_format{ RGB, BGR, GRAY, num };

    BaseCapture():format(num),buffer(NULL){}

    static inline int bpp(out_format format) // bytes per pixel
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

    inline void set_buffer(char* buf, out_format fmt)
    {
        format = fmt;
        buffer = buf;
    }

protected:
    out_format format;
    char* buffer;       // output biffer
};
//-----------------------------------------------------------------------------
#endif//BASE_CAPTURE_H
//-----------------------------------------------------------------------------
