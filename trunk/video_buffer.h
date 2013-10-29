//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   video_buffer.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef VIDEO_BUFFER_H
#define VIDEO_BUFFER_H
//-----------------------------------------------------------------------------
#include "texture_array.h"
#include "bitmap.h"
//-----------------------------------------------------------------------------
class VideoBuffer:public TextureArray
{
public:
    class VideoFrame:public TextureArray::Texture
    {
    public:
        VideoFrame(unsigned char* data, unsigned int width, unsigned int height, GLuint tex_id);
        void update(unsigned char* data, unsigned int offset_x, unsigned int offset_y, unsigned int width, unsigned int height);
    private:
    };

    VideoBuffer(const Bitmap& frame, int layers, unsigned long micsec_delay);

    void update(const Bitmap& frame, unsigned long usec);
    void bind() const;

    float s;
    float t;

protected:
    unsigned int top_index;
private:
    unsigned long last_time;
    unsigned long delay;
};
//-----------------------------------------------------------------------------
#endif//VIDEO_BUFFER_H
//-----------------------------------------------------------------------------
