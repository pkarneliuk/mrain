//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_renderer.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef GL_RENDERER_H
#define GL_RENDERER_H
//-----------------------------------------------------------------------------
#include "gl_context.h"
#include "stuff.h"
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
        union   // T2F
        {
            struct{ GLfloat st[2]; };
            struct{ GLfloat s,t;  };
        };
    };

    struct V3F
    {
        union   // V3F
        {
            struct{ GLfloat xyz[3]; };
            struct{ GLfloat x,y,z;  };
        };
    };

    struct C4F
    {
        union   // C4F
        {
            struct{ GLfloat rgba[4]; };
            struct{ GLfloat r,g,b,a; };
        };
    };

    inline void present(){ GLContext::swap_buffers(); }
    unsigned int draw();
    void reshape(unsigned int width, unsigned int height);

    inline const Version& version ()const   { return gl_version;              }
    inline const GLubyte* renderer()const   { return glGetString(GL_RENDERER);}
    inline const GLubyte* vendor  ()const   { return glGetString(GL_VENDOR);  }

private:
    GLRenderer(class NativeWindow* win);
    ~GLRenderer();

    Version gl_version;
};
//-----------------------------------------------------------------------------
#endif//GL_RENDERER_H
//-----------------------------------------------------------------------------
