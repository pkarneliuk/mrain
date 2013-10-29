//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_context.cpp
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
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
#define WGL_BIND(func) { *((PROC*)(& func )) = wglGetProcAddress( #func ); printf("%p: %s\n", (func), #func); }
#else
#define WGL_BIND(func) { *((PROC*)(& func )) = wglGetProcAddress( #func ); }
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
        // GL_VERSION_1_5
        PFNGLBINDBUFFERPROC glBindBuffer = NULL;
        PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
        PFNGLGENBUFFERSPROC glGenBuffers = NULL;
        PFNGLISBUFFERPROC glIsBuffer = NULL;
        PFNGLBUFFERDATAPROC glBufferData = NULL;
        PFNGLBUFFERSUBDATAPROC glBufferSubData = NULL;
        PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData = NULL;
        PFNGLMAPBUFFERPROC glMapBuffer = NULL;
        PFNGLUNMAPBUFFERPROC glUnmapBuffer = NULL;
        // GL_VERSION_2_0
        PFNGLATTACHSHADERPROC glAttachShader = NULL;
        PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
        PFNGLCOMPILESHADERPROC glCompileShader = NULL;
        PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
        PFNGLCREATESHADERPROC glCreateShader = NULL;
        PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
        PFNGLDELETESHADERPROC glDeleteShader = NULL;
        PFNGLDETACHSHADERPROC glDetachShader = NULL;
        PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
        PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
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
        PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
        PFNGLUSEPROGRAMPROC glUseProgram = NULL;
        PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
        // GL_VERSION_3_0
        PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
        PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
        PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
        PFNGLISVERTEXARRAYPROC glIsVertexArray = NULL;
    }

    namespace Extensions
    {
    }

    namespace WGL
    {
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
        PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT = NULL;
        // WGL_ARB_create_context
        PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
    }
}
//-----------------------------------------------------------------------------
GLContext::GLContext(NativeWindow* win)
{
    hdc = GetDC(win->hwnd);

    PIXELFORMATDESCRIPTOR pfd;

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW |
                     PFD_SUPPORT_OPENGL |
                     PFD_DOUBLEBUFFER   |
                     PFD_GENERIC_FORMAT |
                     PFD_GENERIC_ACCELERATED;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cRedBits   = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits  = 8;
    pfd.cAlphaBits = 8;

    pfd.iLayerType = PFD_MAIN_PLANE;

    pfd.nVersion        = 1;
/*  pfd.cColorBits      = 32;
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
    load_wgl();

    if(wglCreateContextAttribsARB)
    {
        const int attributes[] =
        {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
            WGL_CONTEXT_MINOR_VERSION_ARB, 0,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            WGL_CONTEXT_FLAGS_ARB,         WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
            0
        };

        if(HGLRC hrcattrib = wglCreateContextAttribsARB(hdc, 0, attributes))
        {
            if(wglMakeCurrent(hdc, hrcattrib))
            {
                wglDeleteContext(hrc);
                hrc = hrcattrib;
            }
        }
    }

    wglSwapIntervalEXT(0); // disable V-sync
}

GLContext::~GLContext()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(WindowFromDC(hdc), hdc);
}

bool GLContext::load_ogl()
{
    // GL_VERSION_1_3
    WGL_BIND(glActiveTexture);
    WGL_BIND(glClientActiveTexture);
    // GL_VERSION_1_4
    WGL_BIND(glMultiDrawArrays);
    // GL_VERSION_1_5
    WGL_BIND(glBindBuffer);
    WGL_BIND(glDeleteBuffers);
    WGL_BIND(glGenBuffers);
    WGL_BIND(glIsBuffer);
    WGL_BIND(glBufferData);
    WGL_BIND(glBufferSubData);
    WGL_BIND(glGetBufferSubData);
    WGL_BIND(glMapBuffer);
    WGL_BIND(glUnmapBuffer);
    // GL_VERSION_2_0
    WGL_BIND(glAttachShader);
    WGL_BIND(glBindAttribLocation);
    WGL_BIND(glCompileShader);
    WGL_BIND(glCreateProgram);
    WGL_BIND(glCreateShader);
    WGL_BIND(glDeleteProgram);
    WGL_BIND(glDeleteShader);
    WGL_BIND(glDetachShader);
    WGL_BIND(glDisableVertexAttribArray);
    WGL_BIND(glEnableVertexAttribArray);
    WGL_BIND(glGetProgramiv);
    WGL_BIND(glGetProgramInfoLog);
    WGL_BIND(glGetShaderInfoLog);
    WGL_BIND(glGetShaderiv);
    WGL_BIND(glGetUniformLocation);
    WGL_BIND(glLinkProgram);
    WGL_BIND(glShaderSource);
    WGL_BIND(glValidateProgram);
    WGL_BIND(glUniform1i);
    WGL_BIND(glUniform1f);
    WGL_BIND(glUniform4fv);
    WGL_BIND(glUniformMatrix4fv);
    WGL_BIND(glUseProgram);
    WGL_BIND(glVertexAttribPointer);
    // GL_VERSION_3_0
    WGL_BIND(glBindVertexArray);
    WGL_BIND(glDeleteVertexArrays);
    WGL_BIND(glGenVertexArrays);
    WGL_BIND(glIsVertexArray);

    return true;
}

bool GLContext::load_extensions()
{
    return true;
}

bool GLContext::load_wgl()
{
    WGL_BIND(wglSwapIntervalEXT);
    WGL_BIND(wglGetSwapIntervalEXT);
    // WGL_ARB_create_context
    WGL_BIND(wglCreateContextAttribsARB);

    return true;
}
//-----------------------------------------------------------------------------
