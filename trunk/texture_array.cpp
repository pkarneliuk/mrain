//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   texture_array.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstring>

#include "gl_renderer.h"
#include "texture_array.h"
//-----------------------------------------------------------------------------
TextureArray::TextureArray(unsigned int n):num_textures(n)
{
    texture_id = new GLuint[num_textures];
    glGenTextures (num_textures, texture_id);

    textures = new Texture*[num_textures];
    memset(textures, NULL, sizeof(Texture*) * num_textures);
}

TextureArray::~TextureArray()
{
    for(unsigned int i=0; i<num_textures; i++)
        delete textures[i];			
    delete[] textures;

    glDeleteTextures(num_textures, texture_id);
    delete[] texture_id;
}
//-----------------------------------------------------------------------------
