//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   buffer_object.h
// copyright:   (C) 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H
//-----------------------------------------------------------------------------
#include "gl_renderer.h"
//-----------------------------------------------------------------------------
template <GLenum _target>
class BufferObject
{
protected:
    enum{ target = _target };
public:
    BufferObject(const BufferObject&);   // undefined
    void operator=(const BufferObject&); // undefined

    BufferObject()
    {
        glGenBuffers(1, &buffer);
    }

    ~BufferObject()
    {
        glDeleteBuffers(1, &buffer);
    }

    void bind()
    {
        glBindBuffer(target, buffer);
    }

    static void unbind()
    {
        glBindBuffer(target, 0);
    }

    void create(const GLsizeiptr size, const void* data, GLenum usage)  // alloc + [set_data ...]
    {
        glBufferData(target, size, data, usage);
    }

    void alloc(GLsizeiptr size, GLenum usage)
    {
        glBufferData(target, size, NULL, usage);
    }

    void set_data(GLintptr offset, GLsizeiptr size, const void *data)
    {
        glBufferSubData(target, offset, size, data);
    }

protected:
    GLuint buffer;
};

typedef BufferObject<GL_ARRAY_BUFFER> VBO;

template<typename T>
class VBO_AoS: public VBO // Array of Structures 'T' in Vertex Buffer Object
{
public:
    void create(const unsigned int num, const T* array, GLenum usage)
    {
        VBO::create(num * sizeof(T), array, usage);
    }

    void alloc(const unsigned int num, GLenum usage)
    {
        VBO::alloc(num * sizeof(T), usage);
    }

    void set_data(const unsigned int offset, const unsigned int num, const T* data)
    {
        VBO::set_data(offset * sizeof(T), num * sizeof(T), data);
    }
};

struct NullType {};

template<typename T>
class VBO_SoA: public VBO // Structure 'T' of Arrays in Vertex Buffer Object
{
public:
    void set_data(unsigned int num, T& structure, GLenum usage)
    {
        alloc(num * T::size, usage);
      //  glBufferSubData(target, offset, size, data);
    }

    void set_attribs()
    {
        set_attrib<T>();
      //  glBufferSubData(target, offset, size, data);
    }

protected:


    template<typename A>
    void set_attrib()
    {
        glVertexAttribPointer((GLuint)A::id, A::type::num, A::type::type, GL_FALSE, 0, 0);
        set_attrib<A::next>();
    }
};
//-----------------------------------------------------------------------------
#endif//BUFFER_OBJECT_H
//-----------------------------------------------------------------------------
