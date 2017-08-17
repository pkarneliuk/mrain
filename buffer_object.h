//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "gl_renderer.h"
//------------------------------------------------------------------------------
template <GLenum Target>
class BufferObject : noncopyable
{
protected:
    enum
    {
        target = Target
    };

public:
    class Map : noncopyable
    {
    public:
        Map(GLenum access)
        : address{glMapBuffer(target, access)}
        {
        }

        ~Map() { glUnmapBuffer(target); }

        GLvoid* const address;
    };

    BufferObject() { glGenBuffers(1, &buffer); }

    ~BufferObject() { glDeleteBuffers(1, &buffer); }

    void bind() { glBindBuffer(target, buffer); }

    static void unbind() { glBindBuffer(target, 0); }

    void create(const GLsizeiptr size, const void* data, GLenum usage)
    {
        // alloc + [set_data ...]
        glBufferData(target, size, data, usage);
    }

    void alloc(GLsizeiptr size, GLenum usage)
    {
        glBufferData(target, size, NULL, usage);
    }

    void set_data(GLintptr offset, GLsizeiptr size, const void* data)
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

using VBOBase = BufferObject<GL_ARRAY_BUFFER>;

template <typename T,             // Layout of vertex data
          bool Interleaved = false// true - Array of Structures 'T'
                                  // false - Structure 'T' of Arrays
          >
class VBO : public VBOBase
{
public:
    class Map : public VBOBase::Map
    {
    public:
        Map(GLenum access)
        : VBOBase::Map(access)
        {
        }

        template <std::size_t index>
        auto const address_of(size_t size)
        {
            using Element = T::template index<index>;
            return reinterpret_cast<Element::type* const>(
                ((char*)address) + Element::offset * size);
        }
    };

    void create(const std::size_t num, const void* array, GLenum usage)
    {
        VBOBase::create(num * sizeof(T), array, usage);
    }

    void alloc(const std::size_t num, GLenum usage)
    {
        VBOBase::alloc(num * sizeof(T), usage);
    }

    unsigned int size() const { return VBOBase::size() / sizeof(T); }
};
//------------------------------------------------------------------------------
