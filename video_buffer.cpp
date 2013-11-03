//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   video_buffer.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstring>

#include "video_buffer.h"
//-----------------------------------------------------------------------------
VideoBuffer::VideoFrame::VideoFrame(unsigned char* data, unsigned int width, unsigned int height, GLuint tex_id):TextureArray::Texture(tex_id, GL_TEXTURE_2D)
{
    bind();

//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 //   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
 //   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
}

void VideoBuffer::VideoFrame::update(unsigned char* data, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
}

VideoBuffer::VideoBuffer(const Bitmap& frame, int layers, unsigned long micsec_delay):TextureArray(layers+1 /*for current*/), top_index(0), last_time(micsec_delay), delay(micsec_delay)
{
//    s = 1.0f;//float(frame.width() ) / float(round_pow_2( frame.width()  ));
//    t = 1.0f;//float(frame.height()) / float(round_pow_2( frame.height() ));

    for(unsigned int i=0; i<num_textures; i++)
    {
        textures[i] = new VideoFrame(frame.data(), frame.width(), frame.height(), texture_id[i]);
    }
}

void VideoBuffer::update(const Bitmap& frame, unsigned long usec)
{
    if( (last_time -= usec) <= 0 )
    {
        top_index = (++top_index) % num_textures;
        last_time += delay;
    }

    VideoFrame* video = this->frame();
    video->bind();
    video->update(frame.data(), 0, 0, frame.width(), frame.height());
}
//-----------------------------------------------------------------------------
