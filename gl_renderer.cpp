//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_renderer.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
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
//-----------------------------------------------------------------------------
class Triangle
{
public:
    Triangle(const GLRenderer& renderer):transform(renderer.get_projection())
    {
        const GLfloat v[] = {
                              0.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f,
                              1.0f, 0.0f, 0.0f,
                            }; // vertex

        const GLfloat c[] = {
                              1.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f,
                            }; // color

        glGenVertexArrays(sizeof(vao)/sizeof(vao[0]), vao);
        glGenBuffers(sizeof(vbo)/sizeof(vbo[0]), vbo);


        glBindVertexArray(vao[0]);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(c), c, GL_STATIC_DRAW);
        glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);




        Shader vshader(Shader::Vertex);
        const GLchar* vertex_shader = 
        "#version 130\n"
        "in  vec3 position;"
        "in  vec3 color;"
        "uniform mat4 projectionMatrix;"
        "out vec3 ex_Color;"
        "void main(void)"
        "{"
        "    vec3 v = position;"
                "    v.z -= 1.0;"
        "    gl_Position = projectionMatrix * vec4(v, 1.0);"
        "    ex_Color = color;"
        "}";

        vshader.set_source(vertex_shader);
        vshader.compile();
        vshader.log();

        Shader fshader(Shader::Fragment);
        const GLchar* fertex_shader = 
        "#version 130\n"
        "in  vec3 ex_Color;"
        "out vec4 out_Color;"
        "void main(void)"
        "{"
        "    out_Color = vec4(ex_Color,1.0);"
        "}";

        fshader.set_source(fertex_shader);
        fshader.compile();
        fshader.log();

        program.attach(vshader);
        program.attach(fshader);
        program.bind(0, "position");
        program.bind(1, "color");
//        program.bind(2, "projectionMatrix");

        program.link();
        program.validate();
        program.log();

    }

    ~Triangle()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDeleteBuffers(sizeof(vbo)/sizeof(vbo[0]), vbo);

        glBindVertexArray(0);
        glDeleteVertexArrays(sizeof(vao)/sizeof(vao[0]), vao);
    }

    void draw()
    {

        program.use();

        program.set_uniform_matrix("projectionMatrix", (float*)transform.array);

        glBindVertexArray(vao[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        GPU_Program::use_default();
    }


    GLuint vao[1];
    GLuint vbo[2];

    GPU_Program program;
    const matrix& transform;
};

//-----------------------------------------------------------------------------
GLRenderer::GLRenderer(NativeWindow* win):GLContext(win),triangle(NULL)
{
    printf("GL version:\t%s\n", glGetString(GL_VERSION));
    printf("GLSL version:\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GL renderer:\t%s\n", glGetString(GL_RENDERER));
    printf("GL vendor:\t%s\n", glGetString(GL_VENDOR));

    gl_version = (const char*) glGetString(GL_VERSION);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    unsigned int width;
    unsigned int height;
    win->get_size(&width, &height);
    reshape(width, height);

    triangle = new Triangle(*this);
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

    projection.projection(fov, aspect, znear, zfar);
    modelview.identity();

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

    glLoadIdentity();

    triangle->draw();

    return 0;
}

//-----------------------------------------------------------------------------
