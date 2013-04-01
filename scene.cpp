//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   scene.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "application.h"
#include "matrix.h"
#include "scene.h"
//-----------------------------------------------------------------------------
VideoScreen::VideoScreen(float w, float h):video(NULL),width(w), height(h)
{
}

VideoScreen::~VideoScreen()
{
}

void VideoScreen::draw()
{
    glTranslatef(-32.0f, -24.0f, -30.0f);

    if(video)
    {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_TEXTURE_2D);
        video->bind(GL_MODULATE);
    }
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);         glVertex3f(0.0f, -height, 0.0f);
        glTexCoord2f(0.0f, video->t);     glVertex3f(0.0f, height, 0.0f);
        glTexCoord2f(video->s, video->t); glVertex3f(width*2, height, 0.0f);
        glTexCoord2f(video->s, 0.0f);     glVertex3f(width*2, -height, 0.0f);
    glEnd();
}

void VideoScreen::tick(unsigned long /*usec*/)
{
}

Scene::Scene(GLRenderer* render, const Bitmap* video, const Options& options):atlas(1), buffer(video), frames_stack(NULL), matrix(NULL), screen(NULL), renderer(render)
{
    if(buffer != NULL) // capturing enabled
    {
        frames_stack = new VideoBuffer(*buffer, 0, 10000);

//        screen = new VideoScreen(64,48);
//        screen->set_video(frames_stack);

        if( options[Options::no_shaders] || renderer->version() <= "2.0")
        {
            matrix = new MatrixVideo(128, 112, atlas[0], frames_stack, buffer->width(), buffer->height(), options[Options::vflip], options[Options::hflip]);
        }
        else
        {
            matrix = new MatrixVideoFX(128, 112, atlas[0], frames_stack, buffer->width(), buffer->height(), options[Options::vflip], options[Options::hflip]);
        }
    }
    else matrix = new Matrix(128, 112, atlas[0]);
}

Scene::~Scene()
{
    delete frames_stack;
    delete matrix;
}

unsigned int Scene::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-32.0,26.0,-25.0f);

    matrix->draw();
//    screen->draw();
    return 0;
}

unsigned int Scene::tick(unsigned long usec)
{
    Waiter::tick();

    if(frames_stack)
    {
        frames_stack->update(*buffer, usec);
    }

    matrix->tick(usec);
    return 0;
}
//-----------------------------------------------------------------------------
