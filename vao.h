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






    // Bind
    template<
        typename T,
        typename U,
        unsigned int Index,  // index of vertex attribute array
        unsigned int Offset, // offset at begin of buffer object
        unsigned int SizeOf  // size of structure
    > struct Bind
    {
        static inline void func(const GLuint number)
        {
            glVertexAttribPointer(Index, T::num, T::type, GL_FALSE, 0, (const GLvoid*)(number * Offset));
            glEnableVertexAttribArray(Index);

            Bind<U::element, U::next, Index+1, Offset + sizeof(T), SizeOf>::func(number); // recursive call template
        }
    };

    template<
        typename U,
        unsigned int Index,  // index of vertex attribute array
        unsigned int Offset, // offset at begin of buffer object
        unsigned int SizeOf  // size of structure
    >
    struct Bind<void, U, Index, Offset, SizeOf>
    {
        static inline void func(const GLuint /*unused*/){}
    };


    template <typename Layout, bool Interleaved>
    void bind(VBO<Layout, Interleaved>& /*unused*/, unsigned int num_verticies)
    {
        bind();
    //    Layout::Bind::func(num_verticies);

        Bind<typename Layout::list::element, typename Layout::list::next, 0, 0, sizeof(Layout)>::func(num_verticies);
    }

protected:
    GLuint array;
};
//-----------------------------------------------------------------------------
#endif//VAO_H
//-----------------------------------------------------------------------------