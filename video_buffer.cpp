//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "video_buffer.h"
//------------------------------------------------------------------------------
void VideoBuffer::VideoFrame::update(const std::uint8_t* data,
                                     unsigned int        offset_x,
                                     unsigned int offset_y, unsigned int width,
                                     unsigned int height)
{
    glTexSubImage2D(type(), 0, offset_x, offset_y, width, height, GL_RGB,
                    GL_UNSIGNED_BYTE, data);
}

VideoBuffer::VideoBuffer(const Bitmap& frame, std::size_t layers,
                         unsigned long micsec_delay)
: TextureArray<GL_TEXTURE_2D>(layers + 1 /*for current*/)
, top_index(0)
, last_time(micsec_delay)
, delay(micsec_delay)
{
    for(std::size_t i = 0; i < size(); i++) {
        auto t = (*this)[0];
        t->bind();
        auto type = t->type();
        //    glTexParameteri (type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //   glTexParameteri (type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        //   glTexParameteri (type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(type, 0, GL_RGB, frame.width(), frame.height(), 0, GL_RGB,
                     GL_UNSIGNED_BYTE, frame.data());
        //    glTexSubImage2D(type, 0, 0, 0, width, height, GL_LUMINANCE,
        //    GL_UNSIGNED_BYTE, data);
    }
}

void VideoBuffer::update(const Bitmap& frame, unsigned long usec)
{
    if((last_time -= usec) <= 0)
    {
        ++top_index;
        top_index %= size();
        last_time += delay;
    }

    VideoFrame* video = this->frame();
    video->bind();
    video->update(frame.data(), 0, 0, frame.width(), frame.height());
}
//------------------------------------------------------------------------------
