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





private:
    // Bind
    template<typename T, typename U>
    struct Bind
    {
        static inline void func(GLuint index, GLuint number, size_t offset)
        {
            glVertexAttribPointer(index, T::num, T::type, GL_FALSE, 0, (const GLvoid*)(number * offset));
            glEnableVertexAttribArray(index);

            Bind<U::element, U::next>::func(index+1, number, offset + sizeof(T)); // recursive call template
        }
    };

    template<typename U> struct Bind<void, U> { static inline void func(GLuint, GLuint, size_t){} };

public:

    template <typename Layout, bool Interleaved>
    void bind(VBO<Layout, Interleaved>& /*unused*/, unsigned int num_verticies)
    {
        bind();
        Bind<typename Layout::list::element, typename Layout::list::next>::func(0, num_verticies, 0);
    }

protected:
    GLuint array;
};
//-----------------------------------------------------------------------------
#endif//VAO_H
//-----------------------------------------------------------------------------