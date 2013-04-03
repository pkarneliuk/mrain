//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_renderer.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdio>
#include <cmath>
#include <cstdlib>

#include "native_window.h"
#include "gl_renderer.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GLRenderer::GLRenderer(NativeWindow* win):GLContext(win)
{
    printf("GL version:\t%s\n", glGetString(GL_VERSION));
    printf("GLSL version:\t%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    printf("GL renderer:\t%s\n", glGetString(GL_RENDERER));
    printf("GL vendor:\t%s\n", glGetString(GL_VENDOR));

    gl_version = (const char*) glGetString(GL_VERSION);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    unsigned int width;
    unsigned int height;
    win->get_size(&width, &height);
    reshape(width, height);
}

GLRenderer::~GLRenderer()
{
}

unsigned int GLRenderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0,0,-20);
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 1.0, 0.0f); glVertex3f(0.0, 10.0, 0.0);
        glColor3f(1.0f, 0.0, 0.0f); glVertex3f(0.0, 0.0, 0.0);
        glColor3f(0.0f, 0.0, 1.0f); glVertex3f(10.0, 0.0, 0.0);
    glEnd();

    return 0;
}

void GLRenderer::reshape(unsigned int width, unsigned int height)
{
    glViewport (0, 0, (GLint) width, (GLint) height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat aspect = (GLfloat) width/(GLfloat) height;

    const float fov = 80.0f;
    double cotangent = 1.0/tan(fov /2.0 * 3.1415926 / 180.0);
    double near_plane = 1.0;
    double far_plane  = 1000.0;

    double b = near_plane/cotangent; 
    double a = aspect*b;

    glFrustum(-a, a, -b, b, near_plane, far_plane);
    glMatrixMode(GL_MODELVIEW);
}
//-----------------------------------------------------------------------------
