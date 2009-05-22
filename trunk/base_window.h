//---------- ------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   base_window.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//---------- ------------------------------------------------------------------

//---------- ------------------------------------------------------------------
#ifndef BASE_WINDOW
#define BASE_WINDOW
//---------- ------------------------------------------------------------------
#include "gl_renderer.h"
//---------- ------------------------------------------------------------------
class BaseWindow
{
public:
    enum Mode{ preview, standalone, screensaver };

protected:
    inline BaseWindow(Mode md):mode(md),renderer(NULL){}

    BaseWindow(BaseWindow&);            //undefined
    BaseWindow& operator=(BaseWindow&); //undefined

    inline void resize(unsigned int width, unsigned int height)
    {
        if(renderer) renderer->reshape(width, height);
    }

    inline void paint()
    {
        if(renderer)
        {
            renderer->draw();
            renderer->present();
        }
    }

    const Mode mode;
    GLRenderer* renderer;

    static const char caption[12];
};
//---------- ------------------------------------------------------------------
#endif//BASE_WINDOW
//---------- ------------------------------------------------------------------
