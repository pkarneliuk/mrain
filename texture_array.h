//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   texture_array.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef TEXTURE_ARRAY_H
#define TEXTURE_ARRAY_H
//-----------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>

#ifdef WIN32
#include <windows.h>
#endif// WIN32

#include <GL/gl.h>

#ifdef WIN32
#include "glext.h"
#endif//WIN32

#include "stuff.h"
//-----------------------------------------------------------------------------
class TextureArray
{
protected:
    class Texture
    {
    public:
        Texture(GLuint arg_id, GLenum t):id(arg_id), type(t){}
        virtual ~Texture(){}

        inline void bind(GLint texture_function=0)
        {
            glBindTexture (type, id);
            if(texture_function != 0) glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texture_function);
        }
    private:

        GLuint id;
        GLenum type;
    };

    TextureArray(unsigned int n);
    ~TextureArray();

    Texture** textures;
    GLuint* texture_id;
    unsigned int num_textures;
};
//-----------------------------------------------------------------------------
#endif//TEXTURE_ARRAY_H
//-----------------------------------------------------------------------------
