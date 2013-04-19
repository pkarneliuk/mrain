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

typedef BufferObject<GL_ARRAY_BUFFER> VBOBase;

template<
    typename T,
    bool Interleaved=true // true - Array of Structures 'T' false - Structure 'T' of Arrays
>
class VBO: public VBOBase
{
public:
    void create(const unsigned int num, const void* array, GLenum usage)
    {
        VBOBase::create(num * sizeof(T), array, usage);
    }

    void alloc(const unsigned int num, GLenum usage)
    {
        VBOBase::alloc(num * sizeof(T), usage);
    }
};
//-----------------------------------------------------------------------------
#endif//BUFFER_OBJECT_H
//-----------------------------------------------------------------------------
