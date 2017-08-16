//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "gl_context.h"
#include "stuff.h"
#include <vector>
//------------------------------------------------------------------------------
template <GLenum Type>
class TextureArray : noncopyable
{
public:
    struct TextureID : noncopyable
    {
        TextureID(GLuint i) noexcept
            : id{ i }
        {
        }

        void bind() const noexcept { glBindTexture(Type, id); }
        GLenum type() const noexcept { return Type; }
    private:
        const GLuint id;
    };

    explicit TextureArray(std::size_t n)
    : ids(n)
    {
        glGenTextures((GLsizei)ids.size(), &ids[0]);
    }
    ~TextureArray()
    {
        glDeleteTextures((GLsizei)ids.size(), &ids[0]);
    }

    TextureID* operator[](std::size_t index) const
    {
        return (TextureID*)&(ids[index]);
    }
    std::size_t size() const
    {
        return ids.size();
    }
protected:
    std::vector<GLuint> ids;
};
//------------------------------------------------------------------------------