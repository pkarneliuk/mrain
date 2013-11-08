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
template <GLenum Target>
class BufferObject
{
protected:
    enum{ target = Target };
public:

    class Map
    {
    public:
        Map(GLenum access) : address(glMapBuffer(target, access))
        {
        }

        ~Map()
        {
            glUnmapBuffer(target);
        }

        Map(const Map&);            // undefined
        Map& operator=(const Map&); // undefined

        void * const address;
    };

    BufferObject(const BufferObject&);            // undefined
    BufferObject& operator=(const BufferObject&); // undefined

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

    GLint size() const
    {
        GLint value = 0;
        glGetBufferParameteriv(target, GL_BUFFER_SIZE, &value);
        return value;
    }

protected:
    GLuint buffer;
};

typedef BufferObject<GL_ARRAY_BUFFER> VBOBase;

template<
    typename T,
    bool Interleaved=false // true - Array of Structures 'T' false - Structure 'T' of Arrays
>
class VBO: public VBOBase
{
public:
    class Map : public VBOBase::Map
    {
    public:
        Map(GLenum access) : VBOBase::Map(access)
        {
        }

        ~Map()
        {
        }

        template<typename MemberType, unsigned int index>
        MemberType* const address_of(size_t size)
        {
            return reinterpret_cast<MemberType* const>(((char*)address) + T::index<index>::offset * size);
        }

        Map(const Map&);            // undefined
        Map& operator=(const Map&); // undefined
    };

    void create(const unsigned int num, const void* array, GLenum usage)
    {
        VBOBase::create(num * sizeof(T), array, usage);
    }

    void alloc(const unsigned int num, GLenum usage)
    {
        VBOBase::alloc(num * sizeof(T), usage);
    }

    unsigned int size() const
    {
        return VBOBase::size() / sizeof(T);
    }
};
//-----------------------------------------------------------------------------
#endif//BUFFER_OBJECT_H
//-----------------------------------------------------------------------------
