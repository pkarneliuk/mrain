//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_context.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "gl_context.h"
#include "native_window.h"
//-----------------------------------------------------------------------------
#ifdef _DEBUG
#define PRINT_PROC(a) printf(#a "is %p at %p\n", (a), &(a))
#endif

namespace OpenGL
{
    namespace OGL
    {
        // GL_VERSION_1_3
        PFNGLACTIVETEXTUREARBPROC glActiveTexture = NULL;
        PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture = NULL;
        // GL_VERSION_1_4
        PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = NULL;
        // GL_VERSION_2_0
        PFNGLATTACHSHADERPROC glAttachShader = NULL;
        PFNGLCOMPILESHADERPROC glCompileShader = NULL;
        PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
        PFNGLCREATESHADERPROC glCreateShader = NULL;
        PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
        PFNGLDELETESHADERPROC glDeleteShader = NULL;
        PFNGLDETACHSHADERPROC glDetachShader = NULL;
        PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
        PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
        PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
        PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
        PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
        PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
        PFNGLSHADERSOURCEPROC glShaderSource = NULL;
        PFNGLVALIDATEPROGRAMPROC glValidateProgram = NULL;
        PFNGLUNIFORM1IPROC glUniform1i = NULL;
        PFNGLUNIFORM1FPROC glUniform1f = NULL;
        PFNGLUNIFORM4FVPROC glUniform4fv = NULL;
        PFNGLUSEPROGRAMPROC glUseProgram = NULL;
    }

    namespace Extensions
    {
    }

    namespace WGL
    {
    }
}
//-----------------------------------------------------------------------------
GLContext::GLContext(NativeWindow* win):hwnd(win->hwnd)
{
    hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd;

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                     PFD_DOUBLEBUFFER | PFD_GENERIC_FORMAT | PFD_GENERIC_ACCELERATED;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cRedBits        = 8;
    pfd.cGreenBits      = 8;
    pfd.cBlueBits       = 8;
    pfd.cAlphaBits      = 8;

    pfd.iLayerType = PFD_MAIN_PLANE;

    pfd.nVersion        = 1;
/*  pfd.dwFlags         = 549;
    pfd.iPixelType      = 0;
    pfd.cColorBits      = 32;
    pfd.cRedBits        = 8;
    pfd.cGreenBits      = 8;
    pfd.cBlueBits       = 8;
    pfd.cAlphaBits      = 8;
    pfd.cRedShift       = 16;
    pfd.cGreenShift     = 8;
    pfd.cBlueShift      = 0;
    pfd.cAlphaShift     = 24;
    pfd.cAccumBits      = 64;
    pfd.cAccumRedBits   = 16;
    pfd.cAccumGreenBits = 16;
    pfd.cAccumBlueBits  = 16;
    pfd.cAccumAlphaBits = 16;
    pfd.cDepthBits      = 24;
    pfd.cStencilBits    = 8;
    pfd.cAuxBuffers     = 4;
    pfd.bReserved       = 0;
    pfd.dwVisibleMask   = 0;*/

    int pixelformat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelformat, &pfd);

    hrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hrc);

    load_ogl();
}

GLContext::~GLContext()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(hwnd, hdc);
}

bool GLContext::load_ogl()
{
    // GL_VERSION_1_3
    glActiveTexture       = (PFNGLACTIVETEXTUREARBPROC)     wglGetProcAddress("glActiveTexture");
    glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)  wglGetProcAddress("glClientActiveTexture");
    // GL_VERSION_1_4
    glMultiDrawArrays     = (PFNGLMULTIDRAWARRAYSPROC)      wglGetProcAddress("glMultiDrawArrays");
    // GL_VERSION_2_0
    glAttachShader        = (PFNGLATTACHSHADERPROC)     wglGetProcAddress("glAttachShader");
    glCompileShader       = (PFNGLCOMPILESHADERPROC)    wglGetProcAddress("glCompileShader");
    glCreateProgram       = (PFNGLCREATEPROGRAMPROC)    wglGetProcAddress("glCreateProgram");
    glCreateShader        = (PFNGLCREATESHADERPROC)     wglGetProcAddress("glCreateShader");
    glDeleteProgram       = (PFNGLDELETEPROGRAMPROC)    wglGetProcAddress("glDeleteProgram");
    glDeleteShader        = (PFNGLDELETESHADERPROC)     wglGetProcAddress("glDeleteShader");
    glDetachShader        = (PFNGLDETACHSHADERPROC)     wglGetProcAddress("glDetachShader");
    glGetProgramiv        = (PFNGLGETPROGRAMIVPROC)     wglGetProcAddress("glGetProgramiv");
    glGetProgramInfoLog   = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
    glGetShaderInfoLog    = (PFNGLGETSHADERINFOLOGPROC) wglGetProcAddress("glGetShaderInfoLog");
    glGetShaderiv         = (PFNGLGETSHADERIVPROC)      wglGetProcAddress("glGetShaderiv");
    glGetUniformLocation  = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glLinkProgram         = (PFNGLLINKPROGRAMPROC)      wglGetProcAddress("glLinkProgram");
    glShaderSource        = (PFNGLSHADERSOURCEPROC)     wglGetProcAddress("glShaderSource");
    glValidateProgram     = (PFNGLVALIDATEPROGRAMPROC)  wglGetProcAddress("glValidateProgram");
    glUniform1i           = (PFNGLUNIFORM1IPROC)        wglGetProcAddress("glUniform1i");
    glUniform1f           = (PFNGLUNIFORM1FPROC)        wglGetProcAddress("glUniform1f");
    glUniform4fv          = (PFNGLUNIFORM4FVPROC)       wglGetProcAddress("glUniform4fv");
    glUseProgram          = (PFNGLUSEPROGRAMPROC)       wglGetProcAddress("glUseProgram");

#ifdef _DEBUG
    // GL_VERSION_1_3
    PRINT_PROC(glActiveTexture);
    PRINT_PROC(glClientActiveTexture);
    // GL_VERSION_1_4
    PRINT_PROC(glMultiDrawArrays);
    // GL_VERSION_2_0
    PRINT_PROC(glCreateShader);
    PRINT_PROC(glDeleteShader);
    PRINT_PROC(glShaderSource);
    PRINT_PROC(glCreateProgram);
    PRINT_PROC(glDeleteProgram);
    PRINT_PROC(glAttachShader);
    PRINT_PROC(glDetachShader);
    PRINT_PROC(glUseProgram);
    PRINT_PROC(glClientActiveTexture);
    PRINT_PROC(glGetShaderiv);
    PRINT_PROC(glCompileShader);
    PRINT_PROC(glGetShaderInfoLog);
    PRINT_PROC(glGetProgramiv);
    PRINT_PROC(glLinkProgram);
    PRINT_PROC(glGetProgramInfoLog);
    PRINT_PROC(glValidateProgram);
    PRINT_PROC(glGetUniformLocation);
    PRINT_PROC(glUniform1i);
    PRINT_PROC(glUniform1f);
    PRINT_PROC(glUniform4fv);
#endif

    return true;
}

bool GLContext::load_extensions()
{
    return true;
}

bool GLContext::load_wgl()
{
    return true;
}
//-----------------------------------------------------------------------------
