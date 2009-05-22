//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   gl_context.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef NATIVE_GL_H
#define NATIVE_GL_H
//-----------------------------------------------------------------------------
#include <GL/glx.h>
//-----------------------------------------------------------------------------
class GLContext
{
public:
    static bool check_support(Screen *screen, const char *window_desc, Visual *visual);
    static Visual* choose_format(Display *dpy, int screen_num);

protected:
    GLContext(class NativeWindow* win);
    ~GLContext();

    inline void swap_buffers(){ glXSwapBuffers(dpy, window); }

private:
    GLXContext context;
    Display *dpy;
    Window window;
};
//-----------------------------------------------------------------------------
#endif//NATIVE_GL_H
//-----------------------------------------------------------------------------
