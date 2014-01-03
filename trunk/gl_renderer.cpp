//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_renderer.cpp
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <iostream>

#include "gl_renderer.h"
#include "gpu_program.h"
#include "buffer_object.h"
#include "vertex_data.h"
#include "vao.h"
//-----------------------------------------------------------------------------
class Triangle
{
    typedef VertexData::Layout<VertexData::V3F, VertexData::C3F> V3F_C3F;

public:
    Triangle()
    {
        static_assert(sizeof(V3F_C3F) == 24, "wrong sizeof");
        const unsigned int num_vertices = 3;
        const GLfloat vc_interlaced[] = {
                        // vertex           // color
                        0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
                        1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
                    };

        vbo.bind();
        vbo.create(num_vertices, vc_interlaced, GL_STATIC_DRAW);
            vao.bind();
                vao.bind(vbo, num_vertices, 0);
            vao.unbind();
        vbo.unbind();

        Shader vshader(Shader::Vertex);
        const GLchar* vertex_shader = 
        "#version 130\n"
        "in  vec3 position;"
        "in  vec3 color;"
        "uniform mat4 transform;"
        "out vec3 ex_Color;"
        "void main(void)"
        "{"
        "    gl_Position = transform * vec4(position, 1.0);"
        "    ex_Color = color;"
        "}";

        vshader.set_source(vertex_shader);
        vshader.compile();
        vshader.log();

        Shader fshader(Shader::Fragment);
        const GLchar* fragment_shader = 
        "#version 130\n"
        "in  vec3 ex_Color;"
        "out vec4 out_Color;"
        "void main(void)"
        "{"
        "    out_Color = vec4(ex_Color,1.0);"
        "}";

        fshader.set_source(fragment_shader);
        fshader.compile();
        fshader.log();

        program.attach(vshader);
        program.attach(fshader);
        program.bind(0, "position");
        program.bind(1, "color");

        program.link();
        program.validate();
        program.log();

        model.identity();
        model.translate(vector(0,0,-2));
    }

    void draw(const Transform& transform)
    {
        program.use();
        transform.bind_transform(program, model);
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);
        vao.unbind();

        GPU_Program::use_default();
    }

    VBO<V3F_C3F, true> vbo;
    VAO vao;

    GPU_Program program;
    matrix model;
} * triangle = NULL;
//-----------------------------------------------------------------------------
GLRenderer::GLRenderer(class NativeWindow* win)
    : GLContext(win)
    , transformation(30.0f, 1.0f, 100.0f)
{
    std::clog
    << "GL version:\t"   << glGetString(GL_VERSION)                  << '\n'
    << "GLSL version:\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n'
    << "GL renderer:\t"  << glGetString(GL_RENDERER)                 << '\n'
    << "GL vendor:\t"    << glGetString(GL_VENDOR)                   << std::endl;

    gl_version = (const char*) glGetString(GL_VERSION);

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::reshape(unsigned int width, unsigned int height)
{
    glViewport(0, 0, (GLint) width, (GLint) height);
    transformation.set_viewport(0, 0, width, height);
}

unsigned int GLRenderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return 0;
}
//-----------------------------------------------------------------------------
