//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   vao.h
// copyright:   (C) 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef VAO_H
#define VAO_H
//-----------------------------------------------------------------------------
#include "gl_context.h"
#include "buffer_object.h"
//-----------------------------------------------------------------------------
class VAO
{
public:
    VAO(const VAO&);            // undefined
    void operator=(const VAO&); // undefined

    VAO()
    {
        glGenVertexArrays(1, &array);
    }

    ~VAO()
    {
         glDeleteVertexArrays(1, &array);
    }

    void bind()
    {
        glBindVertexArray(array);
    }

    static void unbind()
    {
        glBindVertexArray(0);
    }






    template <typename Layout, bool Interleaved>
    void bind_VBO(VBO<Layout, Interleaved>& /*unused*/, unsigned int num_verticies)
    {
        bind();
        Layout::Bind::bi(num_verticies);
    }

protected:
    GLuint array;
};
//-----------------------------------------------------------------------------
#endif//VAO_H
//-----------------------------------------------------------------------------