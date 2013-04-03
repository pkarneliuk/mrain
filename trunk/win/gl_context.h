//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_context.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef GL_CONTEXT_H
#define GL_CONTEXT_H
//-----------------------------------------------------------------------------
#include <windows.h>
#include <GL/gl.h>
#include "glext.h"
#include "wglext.h"

#include "../stuff.h"
//-----------------------------------------------------------------------------
class GLContext
{
protected:
    GLContext(class NativeWindow* win);
    ~GLContext();

    inline void swap_buffers(){ SwapBuffers(hdc); }
private:
    bool load_ogl();
    bool load_extensions();
    bool load_wgl();

    HDC     hdc;    // handler device context
    HGLRC   hrc;    // handler render context
};

namespace OpenGL
{
    namespace OGL
    {
        // GL_VERSION_1_3
        extern PFNGLACTIVETEXTUREARBPROC glActiveTexture;
        extern PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;
        // GL_VERSION_1_4
        extern PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
        // GL_VERSION_2_0
        extern PFNGLATTACHSHADERPROC glAttachShader;
        extern PFNGLCOMPILESHADERPROC glCompileShader;
        extern PFNGLCREATEPROGRAMPROC glCreateProgram;
        extern PFNGLCREATESHADERPROC glCreateShader;
        extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
        extern PFNGLDELETESHADERPROC glDeleteShader;
        extern PFNGLDETACHSHADERPROC glDetachShader;
        extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
        extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
        extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
        extern PFNGLGETSHADERIVPROC glGetShaderiv;
        extern PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
        extern PFNGLLINKPROGRAMPROC glLinkProgram;
        extern PFNGLSHADERSOURCEPROC glShaderSource;
        extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;
        extern PFNGLUNIFORM1IPROC glUniform1i;
        extern PFNGLUNIFORM1FPROC glUniform1f;
        extern PFNGLUNIFORM4FVPROC glUniform4fv;
        extern PFNGLUSEPROGRAMPROC glUseProgram;
    }

    namespace Extensions
    {
    }

    namespace WGL
    {
        extern PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT;
        extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
    }
}

using namespace OpenGL::OGL;
//using namespace OpenGL::Extensions;
using namespace OpenGL::WGL;
//-----------------------------------------------------------------------------
#endif//GL_CONTEXT_H
//-----------------------------------------------------------------------------
