//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_context.cpp
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <iostream>

#include "gl_context.h"
#include "native_window.h"
//-----------------------------------------------------------------------------
#ifdef _DEBUG
#define WGL_BIND(func) { *((PROC*)  (&func)) = wglGetProcAddress( #func ); \
                        std::clog << (func) << ": " << #func << std::endl; }
#define LIB_LINK(func) { OpenGL::OGL:: func = :: func;                     \
           std::clog << (OpenGL::OGL::func) << ": " << #func << std::endl; }
#else
#define WGL_BIND(func) { *((PROC*)(& func )) = wglGetProcAddress( #func ); }
#define LIB_LINK(func) { OpenGL::OGL:: func = :: func;                     }
#endif
namespace OpenGL
{
    namespace OGL
    {
        bool load();

        // GL_VERSION_1_0
        PFNGLTEXPARAMETERIPROC glTexParameteri = NULL;
        PFNGLTEXIMAGE2DPROC glTexImage2D = NULL;
        PFNGLCLEARPROC glClear = NULL;
        PFNGLCLEARCOLORPROC glClearColor = NULL;
        PFNGLDEPTHMASKPROC glDepthMask = NULL;
        PFNGLDISABLEPROC glDisable = NULL;
        PFNGLENABLEPROC glEnable = NULL;
        PFNGLBLENDFUNCPROC glBlendFunc = NULL;
        PFNGLGETERRORPROC glGetError = NULL;
        PFNGLGETSTRINGPROC glGetString = NULL;
        PFNGLVIEWPORTPROC glViewport = NULL;
        // GL_VERSION_1_1
        PFNGLDRAWARRAYSPROC glDrawArrays = NULL;
        PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D = NULL;
        PFNGLBINDTEXTUREPROC glBindTexture = NULL;
        PFNGLDELETETEXTURESPROC glDeleteTextures = NULL;
        PFNGLGENTEXTURESPROC glGenTextures = NULL;
        // GL_VERSION_1_3
        PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
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
        bool load();
    }

    namespace WGL
    {
        bool load();

        // WGL_EXT_swap_control
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

    OpenGL::OGL::load();
    OpenGL::WGL::load();

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

extern "C" {
GLAPI void APIENTRY glCullFace (GLenum mode);
GLAPI void APIENTRY glFrontFace (GLenum mode);
GLAPI void APIENTRY glHint (GLenum target, GLenum mode);
GLAPI void APIENTRY glLineWidth (GLfloat width);
GLAPI void APIENTRY glPointSize (GLfloat size);
GLAPI void APIENTRY glPolygonMode (GLenum face, GLenum mode);
GLAPI void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
GLAPI void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
GLAPI void APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
GLAPI void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
GLAPI void APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
GLAPI void APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GLAPI void APIENTRY glDrawBuffer (GLenum mode);
GLAPI void APIENTRY glClear (GLbitfield mask);
GLAPI void APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
GLAPI void APIENTRY glClearStencil (GLint s);
GLAPI void APIENTRY glClearDepth (GLdouble depth);
GLAPI void APIENTRY glStencilMask (GLuint mask);
GLAPI void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
GLAPI void APIENTRY glDepthMask (GLboolean flag);
GLAPI void APIENTRY glDisable (GLenum cap);
GLAPI void APIENTRY glEnable (GLenum cap);
GLAPI void APIENTRY glFinish (void);
GLAPI void APIENTRY glFlush (void);
GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
GLAPI void APIENTRY glLogicOp (GLenum opcode);
GLAPI void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
GLAPI void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
GLAPI void APIENTRY glDepthFunc (GLenum func);
GLAPI void APIENTRY glPixelStoref (GLenum pname, GLfloat param);
GLAPI void APIENTRY glPixelStorei (GLenum pname, GLint param);
GLAPI void APIENTRY glReadBuffer (GLenum mode);
GLAPI void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
GLAPI void APIENTRY glGetBooleanv (GLenum pname, GLboolean *params);
GLAPI void APIENTRY glGetDoublev (GLenum pname, GLdouble *params);
GLAPI GLenum APIENTRY glGetError (void);
GLAPI void APIENTRY glGetFloatv (GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *params);
GLAPI const GLubyte * APIENTRY glGetString (GLenum name);
GLAPI void APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
GLAPI void APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
GLAPI void APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
GLAPI void APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
GLAPI GLboolean APIENTRY glIsEnabled (GLenum cap);
GLAPI void APIENTRY glDepthRange (GLdouble near, GLdouble far);
GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
} // extern "C"

bool OpenGL::OGL::load()
{
    // GL_VERSION_1_0
    LIB_LINK(glTexParameteri);
    LIB_LINK(glTexImage2D);
    LIB_LINK(glClear);
    LIB_LINK(glClearColor);
    LIB_LINK(glDepthMask);
    LIB_LINK(glDisable);
    LIB_LINK(glEnable);
    LIB_LINK(glBlendFunc);
    LIB_LINK(glGetError);
    LIB_LINK(glGetString);
    LIB_LINK(glViewport);
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

bool OpenGL::Extensions::load()
{
    return true;
}

bool OpenGL::WGL::load()
{
    // WGL_EXT_swap_control
    WGL_BIND(wglSwapIntervalEXT);
    WGL_BIND(wglGetSwapIntervalEXT);
    // WGL_ARB_create_context
    WGL_BIND(wglCreateContextAttribsARB);

    return true;
}
//-----------------------------------------------------------------------------
