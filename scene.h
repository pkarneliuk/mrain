//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "buffer_object.h"
#include "capture.h"
#include "matrix.h"
#include "stuff.h"
#include "texture_atlas.h"
#include "vao.h"
#include "vertex_data.h"
#include "video_buffer.h"
//------------------------------------------------------------------------------
class Options;
class VideoScreen : noncopyable
{
    struct V2F_T2F
    {
        VertexData::V2F position;
        VertexData::T2F texcoord;
    };

public:
    VideoScreen(float w, float h);

    void set_video(const VideoBuffer* buffer) { video = buffer; }

    void draw(const Transform& transform);
    void tick(unsigned long usec);

private:
    const VideoBuffer* video;
    float              width;
    float              height;

    VBO<V2F_T2F> vbo;
    VAO          vao;

    GPU_Program program;
    matrix      model;
};

class Scene : noncopyable
{
public:
    Scene(GLRenderer* render, Capture* capture, const Options& options);

    unsigned int draw();
    unsigned int tick(unsigned long usec);

    void present() { renderer->present(); }

    TextureAtlas        atlas;
    BaseCapture::Buffer frame;

    std::unique_ptr<VideoBuffer> frames_stack;
    std::unique_ptr<Matrix>      matrix;
    std::unique_ptr<VideoScreen> screen;

    GLRenderer* renderer;
};
//------------------------------------------------------------------------------
