//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   scene.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "application.h"
#include "matrix.h"
#include "scene.h"
//-----------------------------------------------------------------------------
VideoScreen::VideoScreen(float w, float h):video(NULL), width(w), height(h)
{
    const unsigned int num_vertices = 4;
    const float half_w = w / 2.0f;
    const float half_h = h / 2.0f;

    V2F_T2F vdata[num_vertices] = {
        { -half_w, -half_h,   0.0f, 0.0f },
        { -half_w,  half_h,   0.0f, 1.0f },
        {  half_w, -half_h,   1.0f, 0.0f },
        {  half_w,  half_h,   1.0f, 1.0f },
    };

    vbo.bind();
    vbo.create(num_vertices, vdata, GL_STATIC_DRAW);
        vao.bind();
        vao.set_pointer<VertexData::V2F>(position_id, stride, 0);
        vao.set_pointer<VertexData::T2F>(texcoord_id, stride, sizeof(VertexData::V2F));
        vao.unbind();
    vbo.unbind();

    Shader vshader(Shader::Vertex);
    const GLchar* vertex_shader = 
    "#version 130\n"
    "uniform mat4 transform;"
    "in  vec2 position;"
    "in  vec2 texcoord;"
    "out vec2 ex_texcoord;"
    "void main(void)"
    "{"
    "    gl_Position = transform * vec4(position, 0.0, 1.0);"
    "    ex_texcoord = texcoord;"
    "}";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    Shader fshader(Shader::Fragment);
    const GLchar* fragment_shader = 
    "#version 130\n"
    "uniform sampler2D video;"
    "in  vec2 ex_texcoord;"
    "out vec4 fragment;"
    "void main(void)"
    "{"
    "    fragment = texture(video, ex_texcoord);"
    "}";

    fshader.set_source(fragment_shader);
    fshader.compile();
    fshader.log();

    program.attach(vshader);
    program.attach(fshader);
    program.bind(position_id, "position");
    program.bind(texcoord_id, "texcoord");

    program.link();
    program.validate();
    program.log();

    model.identity();
    model.translate(vector(0,0,-200));
}

VideoScreen::~VideoScreen()
{
}

void VideoScreen::draw(const Transform& transform)
{
    if(video)
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        video->frame()->bind();
    }

    program.use();
        transform.bind_transform(program, model);
        program.set_sampler("video", 0);
        vao.bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        vao.unbind();
    program.use_default();
}

void VideoScreen::tick(unsigned long /*usec*/)
{
}

Scene::Scene(GLRenderer* render, Capture* capture, const Options& options):atlas(1), frame(capture, Capture::RGB), frames_stack(NULL), matrix(NULL), screen(NULL), renderer(render)
{
//    frame.load("test.bmp");
//    frame.dump("tmp.bmp");

    if(frame.data() != NULL) // capturing enabled
    {
        frames_stack = new VideoBuffer(frame, 0, 10000);

//        screen = new VideoScreen(frame.width(), frame.height());
//        screen->set_video(frames_stack);

        matrix = new MatrixVideo(128, 112, atlas[0], frames_stack, frame.width(), frame.height(), options[Options::vflip], options[Options::hflip]);
    }
    else matrix = new Matrix(128, 112, atlas[0]);

    matrix->build_program();
}

Scene::~Scene()
{
    delete screen;
    delete frames_stack;
    delete matrix;
}

unsigned int Scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    renderer->draw();
    if(matrix) matrix->draw(renderer->get_transform());
    if(screen) screen->draw(renderer->get_transform());
    return 0;
}

unsigned int Scene::tick(unsigned long usec)
{
    Waiter::tick();

    if(frames_stack)
    {
        frames_stack->update(frame, usec);
    }

    matrix->tick(usec);
    return 0;
}
//-----------------------------------------------------------------------------
