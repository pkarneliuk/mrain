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
#include <GL/glcorearb.h>
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
        // GL_VERSION_1_0
        extern PFNGLTEXPARAMETERIPROC glTexParameteri;
        extern PFNGLTEXIMAGE2DPROC glTexImage2D;
        extern PFNGLCLEARPROC glClear;
        extern PFNGLCLEARCOLORPROC glClearColor;
        extern PFNGLDISABLEPROC glDisable;
        extern PFNGLENABLEPROC glEnable;
        extern PFNGLBLENDFUNCPROC glBlendFunc;
        extern PFNGLGETERRORPROC glGetError;
        extern PFNGLGETSTRINGPROC glGetString;
        extern PFNGLVIEWPORTPROC glViewport;
        // GL_VERSION_1_1
        extern PFNGLDRAWARRAYSPROC glDrawArrays;
        extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;
        extern PFNGLBINDTEXTUREPROC glBindTexture;
        extern PFNGLDELETETEXTURESPROC glDeleteTextures;
        extern PFNGLGENTEXTURESPROC glGenTextures;
        // GL_VERSION_1_3
        extern PFNGLACTIVETEXTUREPROC glActiveTexture;
        // GL_VERSION_1_4
        extern PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
        // GL_VERSION_1_5
        extern PFNGLBINDBUFFERPROC glBindBuffer;
        extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;
        extern PFNGLGENBUFFERSPROC glGenBuffers;
        extern PFNGLISBUFFERPROC glIsBuffer;
        extern PFNGLBUFFERDATAPROC glBufferData;
        extern PFNGLBUFFERSUBDATAPROC glBufferSubData;
        extern PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData;
        extern PFNGLMAPBUFFERPROC glMapBuffer;
        extern PFNGLUNMAPBUFFERPROC glUnmapBuffer;
        // GL_VERSION_2_0
        extern PFNGLATTACHSHADERPROC glAttachShader;
        extern PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
        extern PFNGLCOMPILESHADERPROC glCompileShader;
        extern PFNGLCREATEPROGRAMPROC glCreateProgram;
        extern PFNGLCREATESHADERPROC glCreateShader;
        extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
        extern PFNGLDELETESHADERPROC glDeleteShader;
        extern PFNGLDETACHSHADERPROC glDetachShader;
        extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
        extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
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
        extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
        extern PFNGLUSEPROGRAMPROC glUseProgram;
        extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
        // GL_VERSION_3_0
        extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
        extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
        extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
        extern PFNGLISVERTEXARRAYPROC glIsVertexArray;
    }

    namespace Extensions
    {
    }

    namespace WGL
    {
        extern PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT;
        extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
        // WGL_ARB_create_context
        extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    }
}

using namespace OpenGL::OGL;
using namespace OpenGL::Extensions;
using namespace OpenGL::WGL;
//-----------------------------------------------------------------------------
#endif//GL_CONTEXT_H
//-----------------------------------------------------------------------------
