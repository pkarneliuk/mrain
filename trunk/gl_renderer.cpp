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
#include "buffer_object.h"
#include "gpu_program.h"
//-----------------------------------------------------------------------------

template<typename T, typename U>
struct Attrib
{
    typedef T type;
    typedef U next;
    enum {
        size = sizeof(T) + U::size,
        length = 1 + U::length,
        id   = 1 + U::id,
    };
};

//struct NullType {};

template<typename U>
struct Attrib<NullType, U>
{
    typedef NullType type;
    enum {
        size   = 0,
        length = 0,
        id     = 0,
    };
};

template <typename M1=NullType,
          typename M2=NullType,
          typename M3=NullType,
          typename M4=NullType,
          typename M5=NullType,
          typename M6=NullType,
          typename M7=NullType,
          typename M8=NullType>
class Elements: public Attrib<M1, 
                       Attrib<M2, 
                       Attrib<M3, 
                       Attrib<M4, 
                       Attrib<M5, 
                       Attrib<M6, 
                       Attrib<M7, 
                       Attrib<M8, 
                       Attrib<NullType, NullType>  > > > >  > > > >
{
};


class Triangle
{
public:
    Triangle(const Transform& t):transform(t)
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

        ///!!!!!! type, offset, size, data

        typedef Elements<GLRenderer::V3F, GLRenderer::C3F, GLRenderer::T2F> W;

       // W::

        struct V
        {
            GLRenderer::V3F v;
            GLRenderer::C3F c;
            GLRenderer::T2F t;
        };

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

        program.link();
        program.validate();
        program.log();

        model.identity();
        model.translate(vector(0,0,-2));
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

        transform.bind_to(program, model);

        glBindVertexArray(vao[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        GPU_Program::use_default();
    }


    GLuint vao[1];
    GLuint vbo[2];

    GPU_Program program;
    matrix model;
    const Transform& transform;
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

    triangle = new Triangle(get_transform());
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

    glLoadIdentity();

    triangle->draw();

    return 0;
}

//-----------------------------------------------------------------------------
