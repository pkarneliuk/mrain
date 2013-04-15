//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_renderer.h
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef GL_RENDERER_H
#define GL_RENDERER_H
//-----------------------------------------------------------------------------
#include "blas.h"
#include "gl_context.h"
#include "stuff.h"
#include "transform.h"
//-----------------------------------------------------------------------------
class GLRenderer:private GLContext
{
    friend class AppWindow;
public:

    struct T2F_V3F_C4F
    {
        union   // T2F
        {
            struct{ GLfloat st[2]; };
            struct{ GLfloat s,t;   };
        };

        union   // V3F
        {
            struct{ GLfloat xyz[3]; };
            struct{ GLfloat x,y,z;  };
        };

        union   // C4F
        {
            struct{ GLfloat rgba[4]; };
            struct{ GLfloat r,g,b,a; };
        };
    };

    struct T2F
    {
        enum{ num=2, type=GL_FLOAT    };
        union   // T2F
        {
            struct{ GLfloat st[num]; };
            struct{ GLfloat s,t;     };
        };
    };

    struct V3F
    {
        enum{ num=3, type=GL_FLOAT    };
        union
        {
            struct{ GLfloat xyz[num]; };
            struct{ GLfloat x,y,z;    };
        };
    };

    struct C3F
    {
        enum{ num=3, type=GL_FLOAT    };
        union
        {
            struct{ GLfloat rgb[num]; };
            struct{ GLfloat r,g,b;    };
        };
    };

    struct C4F
    {
        enum{ num=4, type=GL_FLOAT     };
        union
        {
            struct{ GLfloat rgba[num]; };
            struct{ GLfloat r,g,b,a;   };
        };
    };

    inline const Version& version ()const   { return gl_version;              }
    inline const GLubyte* renderer()const   { return glGetString(GL_RENDERER);}
    inline const GLubyte* vendor  ()const   { return glGetString(GL_VENDOR);  }

    inline void present(){ GLContext::swap_buffers(); }
    void reshape(unsigned int width, unsigned int height);
    unsigned int draw();

    const Transform& get_transform() { return transformation; }

private:
    GLRenderer(class NativeWindow* win);
    ~GLRenderer();

    Version gl_version;
    Transform transformation;
    class Triangle* triangle;
};
//-----------------------------------------------------------------------------
#endif//GL_RENDERER_H
//-----------------------------------------------------------------------------
