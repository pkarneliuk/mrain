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
    friend class NativeWindow;
public:

    inline const Version& version () const { return gl_version;              }
    inline const GLubyte* renderer() const { return glGetString(GL_RENDERER);}
    inline const GLubyte* vendor  () const { return glGetString(GL_VENDOR);  }

    inline void present(){ GLContext::swap_buffers(); }
    void reshape(unsigned int width, unsigned int height);
    unsigned int draw();

    const Transform& get_transform() { return transformation; }

private:
    GLRenderer(class NativeWindow* win);
    ~GLRenderer();

    Version gl_version;
    Transform transformation;
};
//-----------------------------------------------------------------------------
#endif//GL_RENDERER_H
//-----------------------------------------------------------------------------
