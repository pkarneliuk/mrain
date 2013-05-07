//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_renderer.cpp
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdio>
#include <cmath>
#include <cstdlib>

#include "blas.h"
#include "native_window.h"
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
        transform.bind_to(program, model);
        vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        GPU_Program::use_default();
    }

    VBO<V3F_C3F, true> vbo;
    VAO vao;

    GPU_Program program;
    matrix model;
} * triangle = NULL;

//-----------------------------------------------------------------------------
GLRenderer::GLRenderer(NativeWindow* win):GLContext(win)
{
    printf("GL version:\t%s\n", glGetString(GL_VERSION));
    printf("GLSL version:\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GL renderer:\t%s\n", glGetString(GL_RENDERER));
    printf("GL vendor:\t%s\n", glGetString(GL_VENDOR));

    gl_version = (const char*) glGetString(GL_VERSION);

    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    unsigned int width;
    unsigned int height;
    win->get_size(&width, &height);
    reshape(width, height);

    triangle = new Triangle();
}

GLRenderer::~GLRenderer()
{
    delete triangle;
}

void GLRenderer::reshape(unsigned int width, unsigned int height)
{
    glViewport (0, 0, (GLint) width, (GLint) height);

    const float pi = 3.14159265359f;
    const float fov = 80.0f;
    const float aspect = (float) width/(float) height;
    const float znear = 1.0f;
    const float zfar  = 1000.0f;

    transformation.set_perspective(fov, aspect, znear, zfar);

    GLdouble b = znear * tan( fov / 360.0 * pi );
    GLdouble a = b * aspect;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-a, a, -b, b, znear, zfar);
    glMatrixMode(GL_MODELVIEW);
}


unsigned int GLRenderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    triangle->draw(transformation);

    return 0;
}
//-----------------------------------------------------------------------------
