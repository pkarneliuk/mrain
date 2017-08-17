//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
// clang-format off
#include <windows.h>
#include <GL/glcorearb.h>
// clang-format on
#include "../stuff.h"
#include "wglext.h"
//------------------------------------------------------------------------------
class GLContext : noncopyable
{
protected:
    GLContext(class NativeWindow* win);
    ~GLContext();

    inline void swap_buffers() { SwapBuffers(hdc); }

private:
    HDC   hdc;// handler device context
    HGLRC hrc;// handler render context
};

namespace OpenGL
{
    namespace OGL
    {
        // GL_VERSION_1_0
        extern PFNGLTEXPARAMETERIPROC glTexParameteri;
        extern PFNGLTEXIMAGE2DPROC    glTexImage2D;
        extern PFNGLCLEARPROC         glClear;
        extern PFNGLCLEARCOLORPROC    glClearColor;
        extern PFNGLDEPTHMASKPROC     glDepthMask;
        extern PFNGLDISABLEPROC       glDisable;
        extern PFNGLENABLEPROC        glEnable;
        extern PFNGLBLENDFUNCPROC     glBlendFunc;
        extern PFNGLGETERRORPROC      glGetError;
        extern PFNGLGETSTRINGPROC     glGetString;
        extern PFNGLVIEWPORTPROC      glViewport;
        // GL_VERSION_1_1
        extern PFNGLDRAWARRAYSPROC     glDrawArrays;
        extern PFNGLTEXSUBIMAGE2DPROC  glTexSubImage2D;
        extern PFNGLBINDTEXTUREPROC    glBindTexture;
        extern PFNGLDELETETEXTURESPROC glDeleteTextures;
        extern PFNGLGENTEXTURESPROC    glGenTextures;
        // GL_VERSION_1_3
        extern PFNGLACTIVETEXTUREPROC glActiveTexture;
        // GL_VERSION_1_4
        extern PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays;
        // GL_VERSION_1_5
        extern PFNGLBINDBUFFERPROC           glBindBuffer;
        extern PFNGLDELETEBUFFERSPROC        glDeleteBuffers;
        extern PFNGLGENBUFFERSPROC           glGenBuffers;
        extern PFNGLISBUFFERPROC             glIsBuffer;
        extern PFNGLBUFFERDATAPROC           glBufferData;
        extern PFNGLBUFFERSUBDATAPROC        glBufferSubData;
        extern PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData;
        extern PFNGLMAPBUFFERPROC            glMapBuffer;
        extern PFNGLUNMAPBUFFERPROC          glUnmapBuffer;
        extern PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv;
        // GL_VERSION_2_0
        extern PFNGLATTACHSHADERPROC             glAttachShader;
        extern PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation;
        extern PFNGLCOMPILESHADERPROC            glCompileShader;
        extern PFNGLCREATEPROGRAMPROC            glCreateProgram;
        extern PFNGLCREATESHADERPROC             glCreateShader;
        extern PFNGLDELETEPROGRAMPROC            glDeleteProgram;
        extern PFNGLDELETESHADERPROC             glDeleteShader;
        extern PFNGLDETACHSHADERPROC             glDetachShader;
        extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
        extern PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray;
        extern PFNGLGETPROGRAMIVPROC             glGetProgramiv;
        extern PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog;
        extern PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog;
        extern PFNGLGETSHADERIVPROC              glGetShaderiv;
        extern PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation;
        extern PFNGLLINKPROGRAMPROC              glLinkProgram;
        extern PFNGLSHADERSOURCEPROC             glShaderSource;
        extern PFNGLVALIDATEPROGRAMPROC          glValidateProgram;
        extern PFNGLUNIFORM1IPROC                glUniform1i;
        extern PFNGLUNIFORM1FPROC                glUniform1f;
        extern PFNGLUNIFORM4FVPROC               glUniform4fv;
        extern PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv;
        extern PFNGLUSEPROGRAMPROC               glUseProgram;
        extern PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer;
        // GL_VERSION_3_0
        extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;
        // GL_ARB_vertex_array_object
        extern PFNGLBINDVERTEXARRAYPROC    glBindVertexArray;
        extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
        extern PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays;
        extern PFNGLISVERTEXARRAYPROC      glIsVertexArray;
    }

    namespace Extensions
    {
    }

    namespace WGL
    {
        // WGL_EXT_swap_control
        extern PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT;
        extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;
        // WGL_ARB_create_context
        extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
    }
}

using namespace OpenGL::OGL;
using namespace OpenGL::Extensions;
using namespace OpenGL::WGL;
//------------------------------------------------------------------------------
