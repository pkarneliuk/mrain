//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "texture_array.h"
//------------------------------------------------------------------------------
class TextureAtlas : public TextureArray<GL_TEXTURE_2D>
{
public:
    using Texture = TextureArray<GL_TEXTURE_2D>::TextureID;

    TextureAtlas(std::size_t n);
};
//------------------------------------------------------------------------------
