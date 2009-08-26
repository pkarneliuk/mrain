//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   video_buffer.cpp
// copyright:   (C) 2008á 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstring>

#include "video_buffer.h"
//-----------------------------------------------------------------------------
VideoBuffer::VideoFrame::VideoFrame(char* data, unsigned int width, unsigned int height, GLuint tex_id):TextureArray::Texture(tex_id, GL_TEXTURE_2D)
{
    bind();

    unsigned int tex_width = round_pow_2 ( width  );
    unsigned int tex_height = round_pow_2( height );

    char* dummy = new char[tex_width*tex_height];

    memset (dummy, 0x20, tex_width*tex_height);

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE8, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
//    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
    delete[] dummy;
}

void VideoBuffer::VideoFrame::update(char* data, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height)
{
    glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
}

VideoBuffer::VideoBuffer(const Bitmap& frame, int layers, unsigned long micsec_delay):TextureArray(layers+1 /*for current*/), top_index(0), last_time(micsec_delay), delay(micsec_delay)
{
    s = 1.0f;//float(frame.width() ) / float(round_pow_2( frame.width()  ));
    t = 1.0f;//float(frame.height()) / float(round_pow_2( frame.height() ));

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

    VideoFrame* video = (VideoFrame*)textures[top_index];
    video->bind();
    video->update(frame.data(), 0, 0, frame.width(), frame.height());
}

void VideoBuffer::bind(GLint texture_function)const
{
    textures[top_index]->bind(texture_function);
}
//-----------------------------------------------------------------------------
