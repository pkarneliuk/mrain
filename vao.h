//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "buffer_object.h"
#include "gl_context.h"
#include "stuff.h"
//------------------------------------------------------------------------------
class VAO : noncopyable
{
public:
    VAO() { glGenVertexArrays(1, &array); }

    ~VAO() { glDeleteVertexArrays(1, &array); }

    void bind() { glBindVertexArray(array); }

    static void unbind() { glBindVertexArray(0); }

private:
    // Bind
    template <typename T, typename U>
    struct Bind
    {
        static inline void interleaved(GLsizei stride, GLuint index,
                                       size_t offset)
        {
            if(T::type == GL_INT + GL_BYTE ||
               T::type == GL_INT + GL_UNSIGNED_BYTE ||
               T::type == GL_INT + GL_SHORT ||
               T::type == GL_INT + GL_UNSIGNED_SHORT ||
               T::type == GL_INT + GL_INT ||
               T::type == GL_INT + GL_UNSIGNED_INT)
            {
                glVertexAttribIPointer(index, T::num, T::type - GL_INT, stride,
                                       (const GLvoid*)(offset));
            }
            else
            {
                glVertexAttribPointer(index, T::num, T::type, GL_FALSE, stride,
                                      (const GLvoid*)(offset));
            }
            glEnableVertexAttribArray(index);

            Bind<typename U::element, typename U::next>::interleaved(
                stride, index + 1,
                offset + sizeof(T));// recursive call template
        }

        static inline void serial(GLuint number, GLuint index, size_t offset)
        {
            if(T::type == GL_INT + GL_BYTE ||
               T::type == GL_INT + GL_UNSIGNED_BYTE ||
               T::type == GL_INT + GL_SHORT ||
               T::type == GL_INT + GL_UNSIGNED_SHORT ||
               T::type == GL_INT + GL_INT ||
               T::type == GL_INT + GL_UNSIGNED_INT)
            {
                glVertexAttribIPointer(index, T::num, T::type - GL_INT, 0,
                                       (const GLvoid*)(number * offset));
            }
            else
            {
                glVertexAttribPointer(index, T::num, T::type, GL_FALSE, 0,
                                      (const GLvoid*)(number * offset));
            }
            glEnableVertexAttribArray(index);

            Bind<typename U::element, typename U::next>::serial(
                number, index + 1, offset + sizeof(T));
        }
    };

    template <typename U>
    struct Bind<void, U>
    {
        static inline void interleaved(GLsizei, GLuint, size_t) {}
        static inline void serial(GLuint, GLuint, size_t) {}
    };

    template <typename List>
    struct BindList : Bind<typename List::element, typename List::next>
    {
    };

public:
    template <typename Layout>// interleaved placement in VBO f.e:(VNCVNCVNCVNC)
    void bind(VBO<Layout, true>&, GLuint /*n*/, GLuint first_index)
    {
        using List = BindList<typename Layout::list>;
        List::interleaved(sizeof(Layout), first_index, 0);
    }

    template <typename Layout>// serial placement in VBO f.e:(VVVVNNNNCCCC)
    void bind(VBO<Layout, false>&, GLuint n, GLuint first_index)
    {
        using List = BindList<typename Layout::list>;
        List::serial(n, first_index, 0);
    }

    template <typename T>
    static inline void set_pointer(GLuint index, GLsizei stride, size_t offset)
    {
        glVertexAttribPointer(index, T::num, T::type, GL_FALSE, stride,
                              (const GLvoid*)offset);
        glEnableVertexAttribArray(index);
    }

protected:
    GLuint array;
};
//------------------------------------------------------------------------------
