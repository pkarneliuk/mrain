//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "bitmap.h"
#include "texture_array.h"
//------------------------------------------------------------------------------
class VideoBuffer : public TextureArray<GL_TEXTURE_2D>
{
public:
    class VideoFrame : public TextureID
    {
    public:
        void update(unsigned char* data, unsigned int offset_x,
                    unsigned int offset_y, unsigned int width,
                    unsigned int height);
    };

    VideoBuffer(const Bitmap& frame, std::size_t layers,
                unsigned long micsec_delay);

    inline VideoFrame* frame() const
    {
        return static_cast<VideoFrame*>((*this)[top_index]);
    }
    void update(const Bitmap& bitmap, unsigned long usec);

protected:
    std::size_t top_index;

private:
    unsigned long last_time;
    unsigned long delay;
};
//------------------------------------------------------------------------------
