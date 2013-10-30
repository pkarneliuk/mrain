//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   texture_atlas.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "texture_atlas.h"
#include "matrix_img.h"
#include "gl_renderer.h"
//-----------------------------------------------------------------------------

TextureAtlas::TextureAtlas(unsigned int n):TextureArray(n)
{
    // Loading resources from LARGE RESOURCE PACKAGE or something like that :)
    textures[0] = new TextureAtlas::Texture(matrix_img,matrix_img_width, matrix_img_height,texture_id[0]);
}

TextureAtlas::~TextureAtlas()
{
}

TextureAtlas::Texture::Texture(const unsigned char* raw_bitmap, unsigned int width, unsigned int height, unsigned int tex_id):TextureArray::Texture(tex_id, GL_TEXTURE_2D)
{
    bind();
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, raw_bitmap);
}
//-----------------------------------------------------------------------------
