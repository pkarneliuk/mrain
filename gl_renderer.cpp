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
#include "vao.h"
#include "gpu_program.h"
//-----------------------------------------------------------------------------
namespace Meta
{

            struct NullType;

        template<typename T, typename U>
        struct Node:public T, public U
        {
            typedef T element;
            typedef U next;
            enum {
                size = sizeof(T) + U::size,
                length = 1 + U::length,
            };
        };

        template<typename U>
        struct Node<void, U>
        {
            typedef void element;
            enum {
                size   = 0,
                length = 0,
            };
        };


    template <typename M1=void,
              typename M2=void,
              typename M3=void,
              typename M4=void,
              typename M5=void,
              typename M6=void,
              typename M7=void,
              typename M8=void>
    class Structure
    {

    public:





    public:
        typedef Node<M1,
                Node<M2,
                Node<M3,
                Node<M4,
                Node<M5,
                Node<M6,
                Node<M7,
                Node<M8,
                Node<void, void>  > > > >  > > > > list;

        char dummy[sizeof(list)]; // for valid sizeof
    public:
        Structure(); // undefiend

        // Indexing types
        template<typename List, unsigned int i> struct index;

        template<typename T, typename U, unsigned int i>
        struct index<Node<T, U>, i>
        {
            typedef typename index<U, i - 1>::type type;
        };

        template<typename T, typename U>
        struct index<Node<T,U>, 0>
        {
            typedef T type;
        };

    public:
        // Bind
        template<
            typename List,       // list of data types in Nodes<T,U>
            unsigned int Index,  // index of vertex attribute array
            unsigned int Offset, // offset at begin of buffer object
            unsigned int SizeOf  // size of structure
        > struct bind;

        template<typename T, typename U, unsigned int Index, unsigned int Offset, unsigned int SizeOf>
        struct bind<Node<T, U>, Index, Offset, SizeOf>
        {
            static inline void bi(const GLuint number)
            {
                glVertexAttribPointer(Index, T::num, T::type, GL_FALSE, 0, (const GLvoid*)(number * Offset));
                glEnableVertexAttribArray(Index);

                bind<U, Index+1, Offset + sizeof(T), SizeOf>::bi(number); // recursive call template
            }
        };

        template<typename U, unsigned int Index, unsigned int Offset, unsigned int SizeOf>
        struct bind<Node<void, U>, Index, Offset, SizeOf>
        {
            static inline void bi(const GLuint /*number*/){}
        };

        template <unsigned int i>
        struct Index: index<list, i>
        {
        };
        
        struct Bind: bind<list, 0, 0, sizeof(list)>
        {
        };
    };

} // Meta namespace


class Triangle
{
    typedef Meta::Structure<GLRenderer::V3F, GLRenderer::C3F> V3F_C3F;

public:
    Triangle()
    {
        unsigned int num_vertices = 3;
        const GLfloat vc[] = {
                                // vertex
                                0.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                1.0f, 0.0f, 0.0f,
                                // color
                                1.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 1.0f,
                            };

        vbo.bind();
        vbo.create(num_vertices, vc, GL_STATIC_DRAW);

        vao.bind_VBO(vbo, num_vertices);

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
    }

    void draw(const Transform& transform)
    {

        program.use();

        transform.bind_to(program, model);

        vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, 3);

        GPU_Program::use_default();
    }

    VBO<V3F_C3F> vbo;
    VAO vao;

    GPU_Program program;
    matrix model;
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
