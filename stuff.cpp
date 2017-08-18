//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "stuff.h"
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
//------------------------------------------------------------------------------
Random grandom;
Timer  Waiter::timer;
//------------------------------------------------------------------------------
// clang-format off
// The following 'random' numbers are taken from CRC, 18th Edition, page 622.

unsigned int Random::table[Random::table_size] = {
 035340171546, 010401501101, 022364657325, 024130436022, 002167303062, /*  5 */
 037570375137, 037210607110, 016272055420, 023011770546, 017143426366, /* 10 */
 014753657433, 021657231332, 023553406142, 004236526362, 010365611275, /* 14 */
 007117336710, 011051276551, 002362132524, 001011540233, 012162531646, /* 20 */
 007056762337, 006631245521, 014164542224, 032633236305, 023342700176, /* 25 */
 002433062234, 015257225043, 026762051606, 000742573230, 005366042132, /* 30 */
 012126416411, 000520471171, 000725646277, 020116577576, 025765742604, /* 35 */
 007633473735, 015674255275, 017555634041, 006503154145, 021576344247, /* 40 */
 014577627653, 002707523333, 034146376720, 030060227734, 013765414060, /* 45 */
 036072251540, 007255221037, 024364674123, 006200353166, 010126373326, /* 50 */
 015664104320, 016401041535, 016215305520, 033115351014, 017411670323  /* 55 */
};
// clang-format on
Random::Random(unsigned int seed)
{
    if(0 == seed)
        seed = make_seed();

    table[0] += seed;
    for(int i = 1; i < table_size; i++) {
        seed = table[i - 1] * 1001 + seed * 999;
        table[i] += seed;
    }

    index1 = table[0] % table_size;
    index2 = (index1 + 024) % table_size;
}

unsigned int Random::operator()()
{
    register unsigned int r = table[index1] + table[index2];
    table[index1]           = r;
    if(++index1 >= table_size)
        index1 = 0;
    if(++index2 >= table_size)
        index2 = 0;
    return r;
}

float Random::operator()(float f)
{
    float tmp = float(double((*this)()) * double(f) /
                      double((unsigned long)(random_max)));

    return fabs(tmp);
}

unsigned int Random::operator()(unsigned int r) { return (*this)() % r; }

unsigned int Random::make_seed()
{
    return static_cast<unsigned int>(
        Timer::clock::now().time_since_epoch().count());
}

//------------------------------------------------------------------------------

Version::Version(const char* string)
: iversion(0)
{
    unsigned int major   = 0;
    unsigned int minor   = 0;
    unsigned int release = 0;
    if(string)
    {
        sscanf(string, "%d.%d.%d", &major, &minor, &release);
        iversion = char(major) << 16 | char(minor) << 8 | char(release);
    }
}

Version::Version(std::uint8_t major, std::uint8_t minor, std::uint8_t release)
{
    iversion = major << 16 | minor << 8 | release;
}

Version& Version::operator=(const char* str)
{
    Version v(str);
    iversion = v.iversion;
    return *this;
}

bool Version::operator>=(const char* str)
{
    Version v(str);
    return iversion >= v.iversion;
}

bool convert_bmp_2_include_gray_array(char* bmp_file, char* array_name) try
{
    std::ofstream ifs(bmp_file, std::ios_base::in | std::ios_base::binary);

    std::cout << "open " << bmp_file << ':'
              << (ifs.is_open() ? "successed" : "failed") << '\n';
    if(!ifs.is_open())
        return false;

    ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit |
                   std::ifstream::eofbit);

    std::string out_name(array_name);
    out_name += ".h";

    std::ofstream ofs(out_name.c_str(),
                      std::ios_base::out | std::ios_base::binary);
    std::cout << "create " << out_name.c_str() << ':'
              << (ofs.is_open() ? "successed" : "failed") << '\n';
    if(!ofs.is_open())
        return false;

    ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    for(std::string::size_type i = 0; i < out_name.length(); ++i) {
        char c = out_name[i];
        if(isalnum(c))
        {
            out_name[i] = (char)std::toupper(c);
        }
        else
            out_name[i] = '_';
    }

    std::filebuf* const inbuf  = ifs.rdbuf();
    std::filebuf* const outbuf = ofs.rdbuf();

    BMP_FILEHEADER file;
    BMP_INFOHEADER info;

    inbuf->sgetn(reinterpret_cast<char*>(&file), sizeof(file));

    if(file.bfType[0] != 'B' || file.bfType[1] != 'M')
        return false;

    inbuf->sgetn(reinterpret_cast<char*>(&info), sizeof(info));

    const unsigned long width  = info.biWidth;
    const unsigned long height = info.biHeight;
    const unsigned int  bpp    = info.biBitCount / 8;

    ofs << "#ifndef " << out_name.c_str() << "\n#define " << out_name.c_str()
        << '\n';
    ofs << "const unsigned int " << array_name << "_width=" << width << ";\n";
    ofs << "const unsigned int " << array_name << "_height=" << height << ";\n";
    ofs << "const unsigned char " << array_name << "[]={\n";

    ofs << std::hex << std::showbase;

    unsigned char buffer[16 * 4];
    for(unsigned int i = 0; i < (width * height / 16); i++)// 16 values per line
    {
        std::streamsize count =
            inbuf->sgetn(reinterpret_cast<char*>(buffer), 16 * bpp);

        for(std::streamsize k = 0; k < count; k += bpp) {
            unsigned char* pixel = &(buffer[k]);
            //      "   float gray = dot(v.rgb, vec3(0.2125, 0.7154, 0.0721));"
            /*          float red   = float(pixel[0]) * 0.2125f;
                        float green = float(pixel[1]) * 0.7154f;
                        float blue  = float(pixel[2]) * 0.0721f;
                        unsigned int gray = unsigned char((red + green +
               blue)*256.0f);
                        ofs << gray << ',';
            */
            unsigned int color = pixel[0];
            ofs << 0xff - color << ',';
        }

        outbuf->sputc('\n');
    }

    ofs << "};\n#endif//" << out_name.c_str() << '\n';

    return true;
}
catch(std::exception& e)
{
    std::cerr << " error:" << e.what() << std::endl;
    return false;
}
//------------------------------------------------------------------------------
