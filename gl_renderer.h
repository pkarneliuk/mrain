//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "blas.h"
#include "gl_context.h"
#include "stuff.h"
#include "transform.h"
//------------------------------------------------------------------------------
class GLRenderer : private GLContext
{
    friend class AppWindow;

public:
    inline const Version& version() const { return gl_version; }
    inline const GLubyte* renderer() const { return glGetString(GL_RENDERER); }
    inline const GLubyte* vendor() const { return glGetString(GL_VENDOR); }

    inline void present() { GLContext::swap_buffers(); }
    void reshape(unsigned int width, unsigned int height);
    unsigned int draw();

    const Transform& get_transform() const { return transformation; }

private:
    GLRenderer(class NativeWindow* win);

    const Version gl_version;
    Transform     transformation;
};
//------------------------------------------------------------------------------
