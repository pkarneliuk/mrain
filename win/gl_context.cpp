//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "gl_context.h"
#include "native_window.h"
#include <iostream>
//------------------------------------------------------------------------------
namespace OpenGL
{
    namespace OGL
    {
        bool load();

        // GL_VERSION_1_0
        PFNGLTEXPARAMETERIPROC glTexParameteri = NULL;
        PFNGLTEXIMAGE2DPROC    glTexImage2D    = NULL;
        PFNGLCLEARPROC         glClear         = NULL;
        PFNGLCLEARCOLORPROC    glClearColor    = NULL;
        PFNGLDEPTHMASKPROC     glDepthMask     = NULL;
        PFNGLDISABLEPROC       glDisable       = NULL;
        PFNGLENABLEPROC        glEnable        = NULL;
        PFNGLBLENDFUNCPROC     glBlendFunc     = NULL;
        PFNGLGETERRORPROC      glGetError      = NULL;
        PFNGLGETSTRINGPROC     glGetString     = NULL;
        PFNGLVIEWPORTPROC      glViewport      = NULL;
        // GL_VERSION_1_1
        PFNGLDRAWARRAYSPROC     glDrawArrays     = NULL;
        PFNGLTEXSUBIMAGE2DPROC  glTexSubImage2D  = NULL;
        PFNGLBINDTEXTUREPROC    glBindTexture    = NULL;
        PFNGLDELETETEXTURESPROC glDeleteTextures = NULL;
        PFNGLGENTEXTURESPROC    glGenTextures    = NULL;
        // GL_VERSION_1_3
        PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
        // GL_VERSION_1_4
        PFNGLMULTIDRAWARRAYSPROC glMultiDrawArrays = NULL;
        // GL_VERSION_1_5
        PFNGLBINDBUFFERPROC           glBindBuffer           = NULL;
        PFNGLDELETEBUFFERSPROC        glDeleteBuffers        = NULL;
        PFNGLGENBUFFERSPROC           glGenBuffers           = NULL;
        PFNGLISBUFFERPROC             glIsBuffer             = NULL;
        PFNGLBUFFERDATAPROC           glBufferData           = NULL;
        PFNGLBUFFERSUBDATAPROC        glBufferSubData        = NULL;
        PFNGLGETBUFFERSUBDATAPROC     glGetBufferSubData     = NULL;
        PFNGLMAPBUFFERPROC            glMapBuffer            = NULL;
        PFNGLUNMAPBUFFERPROC          glUnmapBuffer          = NULL;
        PFNGLGETBUFFERPARAMETERIVPROC glGetBufferParameteriv = NULL;
        // GL_VERSION_2_0
        PFNGLATTACHSHADERPROC             glAttachShader             = NULL;
        PFNGLBINDATTRIBLOCATIONPROC       glBindAttribLocation       = NULL;
        PFNGLCOMPILESHADERPROC            glCompileShader            = NULL;
        PFNGLCREATEPROGRAMPROC            glCreateProgram            = NULL;
        PFNGLCREATESHADERPROC             glCreateShader             = NULL;
        PFNGLDELETEPROGRAMPROC            glDeleteProgram            = NULL;
        PFNGLDELETESHADERPROC             glDeleteShader             = NULL;
        PFNGLDETACHSHADERPROC             glDetachShader             = NULL;
        PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = NULL;
        PFNGLENABLEVERTEXATTRIBARRAYPROC  glEnableVertexAttribArray  = NULL;
        PFNGLGETPROGRAMIVPROC             glGetProgramiv             = NULL;
        PFNGLGETPROGRAMINFOLOGPROC        glGetProgramInfoLog        = NULL;
        PFNGLGETSHADERINFOLOGPROC         glGetShaderInfoLog         = NULL;
        PFNGLGETSHADERIVPROC              glGetShaderiv              = NULL;
        PFNGLGETUNIFORMLOCATIONPROC       glGetUniformLocation       = NULL;
        PFNGLLINKPROGRAMPROC              glLinkProgram              = NULL;
        PFNGLSHADERSOURCEPROC             glShaderSource             = NULL;
        PFNGLVALIDATEPROGRAMPROC          glValidateProgram          = NULL;
        PFNGLUNIFORM1IPROC                glUniform1i                = NULL;
        PFNGLUNIFORM1FPROC                glUniform1f                = NULL;
        PFNGLUNIFORM4FVPROC               glUniform4fv               = NULL;
        PFNGLUNIFORMMATRIX4FVPROC         glUniformMatrix4fv         = NULL;
        PFNGLUSEPROGRAMPROC               glUseProgram               = NULL;
        PFNGLVERTEXATTRIBPOINTERPROC      glVertexAttribPointer      = NULL;
        // GL_VERSION_3_0
        PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = NULL;
        // GL_ARB_vertex_array_object
        PFNGLBINDVERTEXARRAYPROC    glBindVertexArray    = NULL;
        PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
        PFNGLGENVERTEXARRAYSPROC    glGenVertexArrays    = NULL;
        PFNGLISVERTEXARRAYPROC      glIsVertexArray      = NULL;
    }

    namespace Extensions
    {
        bool load();
    }

    namespace WGL
    {
        bool load();

        // WGL_EXT_swap_control
        PFNWGLSWAPINTERVALEXTPROC    wglSwapIntervalEXT    = NULL;
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
    pfd.nSize    = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER |
                  PFD_GENERIC_FORMAT | PFD_GENERIC_ACCELERATED;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cRedBits   = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits  = 8;
    pfd.cAlphaBits = 8;

    pfd.iLayerType = PFD_MAIN_PLANE;

    pfd.nVersion = 1;
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

    OpenGL::OGL::load();
    OpenGL::WGL::load();

    if(wglCreateContextAttribsARB)
    {
        const int attributes[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                                  4,
                                  WGL_CONTEXT_MINOR_VERSION_ARB,
                                  0,
                                  WGL_CONTEXT_PROFILE_MASK_ARB,
                                  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                  WGL_CONTEXT_FLAGS_ARB,
                                  WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
                                  0};

        if(HGLRC hrcattrib = wglCreateContextAttribsARB(hdc, 0, attributes))
        {
            if(wglMakeCurrent(hdc, hrcattrib))
            {
                wglDeleteContext(hrc);
                hrc = hrcattrib;
            }
        }
    }

    wglSwapIntervalEXT(0);// disable V-sync
}

GLContext::~GLContext()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(WindowFromDC(hdc), hdc);
}

static HMODULE opengl32 = GetModuleHandle("opengl32.dll");

#define DO_WGL_BIND(func)                                                    \
    do                                                                       \
    {                                                                        \
        reinterpret_cast<PROC&>(func) = wglGetProcAddress(#func);            \
        if(func == nullptr)                                                  \
            reinterpret_cast<PROC&>(func) = GetProcAddress(opengl32, #func); \
    } while(0)

#ifdef _DEBUG
#define WGL_BIND(func)                                \
    do                                                \
    {                                                 \
        DO_WGL_BIND(func);                            \
        std::clog << (func) << ": " << #func << '\n'; \
    } while(0)
#else
#define WGL_BIND(func) DO_WGL_BIND(func)
#endif

bool OpenGL::OGL::load()
{
    // GL_VERSION_1_0
    WGL_BIND(glTexParameteri);
    WGL_BIND(glTexImage2D);
    WGL_BIND(glClear);
    WGL_BIND(glClearColor);
    WGL_BIND(glDepthMask);
    WGL_BIND(glDisable);
    WGL_BIND(glEnable);
    WGL_BIND(glBlendFunc);
    WGL_BIND(glGetError);
    WGL_BIND(glGetString);
    WGL_BIND(glViewport);
    // GL_VERSION_1_1
    WGL_BIND(glDrawArrays);
    WGL_BIND(glTexSubImage2D);
    WGL_BIND(glBindTexture);
    WGL_BIND(glDeleteTextures);
    WGL_BIND(glGenTextures);
    // GL_VERSION_1_3
    WGL_BIND(glActiveTexture);
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
    WGL_BIND(glGetBufferParameteriv);
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
    WGL_BIND(glVertexAttribIPointer);
    // GL_ARB_vertex_array_object
    WGL_BIND(glBindVertexArray);
    WGL_BIND(glDeleteVertexArrays);
    WGL_BIND(glGenVertexArrays);
    WGL_BIND(glIsVertexArray);

    return true;
}

bool OpenGL::Extensions::load() { return true; }

bool OpenGL::WGL::load()
{
    // WGL_EXT_swap_control
    WGL_BIND(wglSwapIntervalEXT);
    WGL_BIND(wglGetSwapIntervalEXT);
    // WGL_ARB_create_context
    WGL_BIND(wglCreateContextAttribsARB);

    return true;
}
//------------------------------------------------------------------------------
