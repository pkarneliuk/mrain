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

#include "capture.h"
#include "gl_renderer.h"
#include "matrix.h"
#include "stuff.h"
#include "texture_atlas.h"
#include "video_buffer.h"

#include "buffer_object.h"
#include "vao.h"
#include "vertex_data.h"
//-----------------------------------------------------------------------------
class VideoScreen
{
    struct V2F_T2F
    {
        VertexData::V2F position;
        VertexData::T2F texcoord;
    };

    enum {
        position_id=0,
        texcoord_id=1,
        stride=sizeof(VertexData::V2F)+sizeof(VertexData::T2F),
    };

public:
    VideoScreen(float w, float h);
    ~VideoScreen();

    void set_video(const VideoBuffer* buffer){ video = buffer; }

    void draw(const Transform& transform);
    void tick(unsigned long usec);
private:

    const VideoBuffer* video;
    float width;
    float height;

    VBO<V2F_T2F> vbo;
    VAO vao;

    GPU_Program program;
    matrix model;
};

class Scene
{
public:
    Scene(GLRenderer* render, Capture* capture, const Options& options);
    ~Scene();

    unsigned int draw();
    unsigned int tick(unsigned long usec);

    void present(){ renderer->present(); }

    TextureAtlas atlas;
    BaseCapture::Buffer frame;

    VideoBuffer* frames_stack;
    Matrix* matrix;
    VideoScreen* screen;


    GLRenderer* renderer;
};
//-----------------------------------------------------------------------------
#endif//SCENE_H
//-----------------------------------------------------------------------------
