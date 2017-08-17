//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "stuff.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
//------------------------------------------------------------------------------
class Bitmap
{
public:
    Bitmap()
    : image_width(0)
    , image_height(0)
    {
    }

    unsigned int         width() const { return image_width; }
    unsigned int         height() const { return image_height; }
    unsigned char*       data() { return &(buffer[0]); }
    const unsigned char* data() const { return &(buffer[0]); }

    bool dump(const char* bmp_file) const try
    {
        std::clog << "dump bitmap to: " << bmp_file << std::endl;

        unsigned int dob_width     = (4 - (image_width & 0x3)) & 0x3;
        unsigned int rounded_width = image_width + dob_width;
        unsigned int data_size = rounded_width * image_height * 3;// bpp == 24

        BMP_FILEHEADER file;
        BMP_INFOHEADER info;

        file.bfType[0]   = 'B';
        file.bfType[1]   = 'M';
        file.bfSize      = sizeof(file) + sizeof(info) + data_size;
        file.bfReserved1 = 0;
        file.bfReserved2 = 0;
        file.bfOffBits   = 0;

        info.biSize          = sizeof(info);
        info.biWidth         = image_width;
        info.biHeight        = image_height;
        info.biPlanes        = 1;
        info.biBitCount      = 24;
        info.biCompression   = 0;
        info.biSizeImage     = 0;
        info.biXPelsPerMeter = 1;
        info.biYPelsPerMeter = 1;
        info.biClrUsed       = 0;
        info.biClrImportant  = 0;

        std::ofstream ofs;
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(bmp_file, std::ios_base::out | std::ios_base::binary);

        std::filebuf* const outbuf = ofs.rdbuf();

        outbuf->sputn(reinterpret_cast<const char*>(&file), sizeof(file));
        outbuf->sputn(reinterpret_cast<const char*>(&info), sizeof(info));

        const char dummy[4] = {0};

        // write from bottom to up
        const unsigned char* pixel =
            &(buffer[0]) + (image_height - 1) * (image_width * 3);
        for(unsigned int i = 0; i < image_height; i++) {
            pixel -= outbuf->sputn(reinterpret_cast<const char*>(pixel),
                                   image_width * 3);
            if(dob_width)
            {
                outbuf->sputn(dummy, dob_width);
            }
        }

        return true;
    }
    catch(std::exception& e)
    {
        std::cerr << "couldn't write to file:" << bmp_file
                  << " error:" << e.what() << std::endl;
        return false;
    }

    bool load(const char* bmp_file) try
    {
        BMP_FILEHEADER file = {0};
        BMP_INFOHEADER info = {0};

        std::ofstream ifs;
        ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit |
                       std::ifstream::eofbit);
        ifs.open(bmp_file, std::ios_base::in | std::ios_base::binary);

        std::filebuf* const inbuf = ifs.rdbuf();

        inbuf->sgetn(reinterpret_cast<char*>(&file), sizeof(file));
        inbuf->sgetn(reinterpret_cast<char*>(&info), sizeof(info));

        image_width  = info.biWidth;
        image_height = info.biHeight;

        const size_t data_size  = file.bfSize - (sizeof(file) + sizeof(info));
        const size_t pixel_size = info.biBitCount / 8;
        const size_t line_size  = image_width * pixel_size;
        const size_t dob_width  = (4 - (image_width & 0x3)) & 0x3;

        buffer.resize(data_size);

        // read from bottom to up
        unsigned char* pixel = &(buffer[0]) + (image_height - 1) * line_size;
        for(unsigned int i = 0; i < image_height; i++) {
            pixel -= inbuf->sgetn(reinterpret_cast<char*>(pixel), line_size);
            if(dob_width)
            {
                inbuf->pubseekoff(dob_width, std::ios_base::cur);
            }
        }
        return true;
    }
    catch(std::exception& e)
    {
        buffer.clear();
        std::cerr << "couldn't read from file:" << bmp_file
                  << " error:" << e.what() << std::endl;
        return false;
    }

protected:
    unsigned int               image_width;
    unsigned int               image_height;
    std::vector<unsigned char> buffer;
};
//------------------------------------------------------------------------------
