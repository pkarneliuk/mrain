//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   scene.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef SCENE_H
#define SCENE_H
//-----------------------------------------------------------------------------
#include <cmath>

#include "gl_renderer.h"
#include "matrix.h"
#include "stuff.h"
#include "texture_atlas.h"
#include "video_buffer.h"
//-----------------------------------------------------------------------------
class VideoScreen
{
public:
    VideoScreen(float w, float h);
    ~VideoScreen();

    void set_video(const VideoBuffer* buffer){ video = buffer; }

    void draw();
    void tick(unsigned long usec);
private:

    const VideoBuffer* video;
    float width;
    float height;
};

class Scene
{
public:
    Scene(GLRenderer* render, class Capture* capture, const Options& options);
    ~Scene();

    unsigned int draw();
    unsigned int tick(unsigned long usec);

    void present(){ renderer->present(); }

    TextureAtlas atlas;
    Bitmap target;

    VideoBuffer* frames_stack;
    Matrix* matrix;
    VideoScreen* screen;

    GLRenderer* renderer;
};
//-----------------------------------------------------------------------------
#endif//SCENE_H
//-----------------------------------------------------------------------------
