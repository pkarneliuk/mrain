//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "scene.h"
#include "matrix.h"
#include "options.h"
//------------------------------------------------------------------------------
VideoScreen::VideoScreen(float w, float h)
: video{nullptr}
, width{w}
, height{h}
{
    const std::size_t num    = 4;
    const float       half_w = w / 2.0f;
    const float       half_h = h / 2.0f;

    V2F_T2F vdata[num] = {
        {-half_w, -half_h, 0.0f, 0.0f},
        {-half_w, +half_h, 0.0f, 1.0f},
        {+half_w, -half_h, 1.0f, 0.0f},
        {+half_w, +half_h, 1.0f, 1.0f},
    };

    enum
    {
        position_id = 0,
        texcoord_id = 1,
        stride      = sizeof(VertexData::V2F) + sizeof(VertexData::T2F),
    };

    vbo.bind();
    {
        vbo.create(num, vdata, GL_STATIC_DRAW);
        vao.bind();
        {
            vao.set_pointer<VertexData::V2F>(position_id, stride, 0);
            vao.set_pointer<VertexData::T2F>(texcoord_id, stride,
                                             sizeof(VertexData::V2F));
        }
        vao.unbind();
    }
    vbo.unbind();

    Shader        vshader(Shader::Vertex);
    const GLchar* vertex_shader = R"vs(
    #version 130
    uniform mat4 transform;
    in  vec2 position;
    in  vec2 texcoord;
    out vec2 ex_texcoord;
    void main(void)
    {
        gl_Position = transform * vec4(position, 0.0, 1.0);
        ex_texcoord = texcoord;
    })vs";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    Shader        fshader(Shader::Fragment);
    const GLchar* fragment_shader = R"fs(
    #version 130
    uniform sampler2D video;
    in  vec2 ex_texcoord;
    out vec4 fragment;
    void main(void)
    {
        fragment = texture(video, ex_texcoord);
    })fs";

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
    model.translate(vector(0, 0, -400));
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
    {
        transform.bind_transform(program, model);
        program.set_sampler("video", 0);
        vao.bind();
        {
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
        vao.unbind();
    }
    program.use_default();
}

void VideoScreen::tick(unsigned long /*usec*/) {}

Scene::Scene(GLRenderer* render, Capture* capture, const Options& options)
: atlas(1)
, frame(capture, Capture::RGB)
, renderer(render)
{
    if(frame.data() != nullptr)// capturing enabled
    {
        frames_stack = std::make_unique<VideoBuffer>(frame, 0, 10000);

        // screen = new VideoScreen(frame.width(), frame.height());
        // screen->set_video(frames_stack);

        matrix = std::make_unique<MatrixVideo>(
            128, 96, atlas[0], frames_stack.get(), options[Options::vflip],
            options[Options::hflip]);
    }
    else
        matrix = std::make_unique<Matrix>(128, 96, atlas[0]);

    matrix->build_program();
}

unsigned int Scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //    renderer->draw();
    if(matrix)
        matrix->draw(renderer->get_transform());
    if(screen)
        screen->draw(renderer->get_transform());
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
//------------------------------------------------------------------------------
