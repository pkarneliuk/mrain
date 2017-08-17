//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "stuff.h"
#include <cstring>
#include <string>
//------------------------------------------------------------------------------
class Options : noncopyable
{
private:
    class Opt
    {
        friend class Options;

    public:
        operator std::string() const;
        operator const char* const() const;
        operator bool() const;
        operator int() const;
        operator float() const;
        operator double() const;

        void operator=(const std::string& v);
        void operator=(const char* v);
        void operator=(bool v);
        void operator=(int v);
        void operator=(float v);
        void operator=(double v);

        bool empty() const { return 0 == strlen(value); }

    private:
        Opt(char k, const char* n, const char* v, const char* c);
        Opt()        = default;
        Opt& operator=(const Opt&);

        char              value[128];// string value of option
        char              name[16];  // name of option
        const char* const comment = nullptr;
        char              key;// a character 'X' or '-' if it is not set
    };

public:
    enum opt_name
    {
        scrsvr_mode,
        window_id,
        config,
        width,
        height,
        vflip,
        hflip,
        no_shaders,
        device,
        help,
        num
    };

    Options(const char* default_file_name);

    void parse(int argc, char** argv);// parse command-line
    bool save(const char* filepath = NULL);
    bool load(const char* filepath = NULL);
    int usage() const;

    inline const Opt& operator[](opt_name id) const { return options[id]; }
    inline Opt& operator[](opt_name id) { return options[id]; }

private:
    enum
    {
        max_path = 256
    };

    // private helpers
    int search_name(const char* name);
    int search_key(char key);
    const char* is_short_key(const char* str);

    Opt options[num];

    char default_filepath[max_path];
};
//------------------------------------------------------------------------------
