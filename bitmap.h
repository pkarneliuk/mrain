//---------- ------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   bitmap.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//---------- ------------------------------------------------------------------

//---------- ------------------------------------------------------------------
#ifndef BITMAP_H
#define BITMAP_H
//---------- ------------------------------------------------------------------
#include <cstdio>

#include "stuff.h"
//---------- ------------------------------------------------------------------
class Bitmap
{
public:
    Bitmap():image_width(0), image_height(0), buffer(NULL){}

    ~Bitmap(){ clear(); }

    unsigned int width ()const{ return image_width;  }
    unsigned int height()const{ return image_height; }
    unsigned char* data()const{ return buffer; }

    bool dump(const char* bmp_file)const
    {
        fprintf (stderr, "dump bitmap into %s\n", bmp_file);

        unsigned int dob_width = (4 - (image_width & 0x3)) & 0x3; 
        unsigned int rounded_width = image_width + dob_width; 
        unsigned int data_size = rounded_width*image_height*3;  // bpp == 24

        BMP_FILEHEADER file;
        BMP_INFOHEADER info;

        file.bfType[0] = 'B';
        file.bfType[1] = 'M';
        file.bfSize = sizeof(file) + sizeof(info) + data_size;
        file.bfReserved1 = 0;
        file.bfReserved2 = 0;
        file.bfOffBits = 0;

        info.biSize = sizeof(info); 
        info.biWidth = image_width;
        info.biHeight = image_height;
        info.biPlanes = 1; 
        info.biBitCount = 24; 
        info.biCompression =0; 
        info.biSizeImage = 0; 
        info.biXPelsPerMeter = 1; 
        info. biYPelsPerMeter = 1; 
        info.biClrUsed = 0; 
        info.biClrImportant = 0;

        size_t res = 0;

        FILE* mfd = fopen(bmp_file, "wb");
        res = fwrite(&file, sizeof(file), 1, mfd);
        res = fwrite(&info, sizeof(info), 1, mfd);

        char dummy[4];

        // write from bottom to up
        unsigned char* pixel = buffer + (image_height-1) * (image_width * 3);
        for(unsigned int i=0; i<image_height; i++)
        {
            pixel -= fwrite(pixel, 3, image_width, mfd) * 3;
            res = fwrite(dummy, dob_width, sizeof(char), mfd);
        }
        fclose(mfd);

        return true;
    }

protected:

    inline void clear()
    {
        delete[] buffer;
        buffer = NULL;
    }

    unsigned int image_width;
    unsigned int image_height;
    unsigned char* buffer;
};
//---------- ------------------------------------------------------------------
#endif//BITMAP_H
//---------- ------------------------------------------------------------------
