//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "texture_atlas.h"
#include "gl_renderer.h"
#include "matrix_img.h"
//------------------------------------------------------------------------------
TextureAtlas::TextureAtlas(std::size_t n)
    : TextureArray<GL_TEXTURE_2D>{ n }
{
    auto t = (*this)[0];
    t->bind();
    auto type = t->type();
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(type, 0, GL_RED, matrix_img_width, matrix_img_height, 0, GL_RED,
        GL_UNSIGNED_BYTE, matrix_img);
}
//------------------------------------------------------------------------------
