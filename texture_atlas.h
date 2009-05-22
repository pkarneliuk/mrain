//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   texture_atlas.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H
//-----------------------------------------------------------------------------
#include "texture_array.h"
//-----------------------------------------------------------------------------
class TextureAtlas:public TextureArray
{
public:
    class Texture:public TextureArray::Texture
    {
    public:
        Texture(const unsigned char* raw_bitmap, unsigned int width, unsigned int height, unsigned int tex_id);
    };

    TextureAtlas::Texture* operator[](unsigned int index){ return (TextureAtlas::Texture*)textures[index]; }

    TextureAtlas(unsigned int n);
    ~TextureAtlas();
};
//-----------------------------------------------------------------------------
#endif//TEXTURE_ATLAS_H
//-----------------------------------------------------------------------------
