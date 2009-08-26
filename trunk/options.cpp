//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   options.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------
//Ida Maria
//-----------------------------------------------------------------------------
#include <cassert>
#include <cstdio>
#include <cstring>
#include <fstream>

#include "options.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
Options::Opt::Opt(char k, const char* n, const char* v, const char* c):comment(c), key(k)
{
    assert(sizeof(name)  > strlen(n));
    assert(sizeof(value) > strlen(v));

    strcpy(name,  n);
    strcpy(value, v);
}

Options::Opt& Options::Opt::operator=(const Opt& val)
{
    memcpy(this, &val, sizeof(val));
    return *this;
}

Options::Opt::operator std::string()const
{
    if(NULL == value)
    {
        fprintf (stderr, "resource not found, return default value: empty-string.\n");
        return std::string();
    }
    std::string str(value);
    return str;
}

Options::Opt::operator const char*const()const
{
    return value;
}

Options::Opt::operator bool ()const
{
    if(NULL == value)
    {
        fprintf (stderr, "resource not found, return default value: false.\n");
        return false;
    }

    char buf [100];
    char* tmp = buf;

    for (const char *s = value; *s ; s++)
    *tmp++ = isupper(*s) ? (char)tolower(*s) : *s;
    *tmp = '\0';

    if (!strcmp (buf, "on") || !strcmp (buf, "true") || !strcmp (buf, "yes"))
        return true;

    if (!strcmp (buf,"off") || !strcmp (buf, "false") || !strcmp (buf,"no"))
        return false;

    fprintf (stderr, "resource value must be boolean, not \"%s\".\n", buf);
    return false;
}

Options::Opt::operator int ()const
{
    if(NULL == value)
    {
        fprintf (stderr, "resource not found, return default value: 0.\n");
        return 0;
    }

    const char* s = value;
    while (*s && *s <= ' ') s++;

    int val = 0;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))	// 0x: parse as hex
    {
        if (0 < sscanf (s, "%X", &val))
        {
            return val;
        }
    }
    else                        // else parse as dec
    {
        if (0 < sscanf (s, "%d", &val))
        {
            return val;
        }
    }

    fprintf (stderr, "resource value must be an integer  [%s]\n", value);
    return 0;
}

Options::Opt::operator float ()const
{
    if(NULL == value)
    {
        fprintf (stderr, "resource not found, return default value: 0.0f.\n");
        return 0.0f;
    }

    float val=0.0f;
    if (0 < sscanf (value, " %f", &val))
    {
        return val;
    }
    fprintf (stderr, "resource value must be a float\n");
    return 0.0f;
}

Options::Opt::operator double ()const
{
    if(NULL == value)
    {
        fprintf (stderr, "resource not found, return default value: 0.0.\n");
        return 0.0;
    }

    double val;
    if (0 < sscanf (value, " %lf", &val))
    {
        return val;
    }
    fprintf (stderr, "resource value must be a double\n");
    return 0.0;
}

void Options::Opt::operator=(const std::string& v)
{
    assert(sizeof(value) > strlen(v.c_str()));
    strcpy(value, v.c_str());
}

void Options::Opt::operator=(const char* v)
{
    assert(sizeof(value) > strlen(v));
    strcpy(value, v);
}

void Options::Opt::operator=(bool v)
{
    assert(sizeof(value) > 5);
    strcpy(value, v ? "true" : "false");
}

void Options::Opt::operator=(int v)
{
    assert(sizeof(value) > 16);
    sprintf(value, "%i", v);
}

void Options::Opt::operator=(float v)
{
    assert(sizeof(value) > 16);
    sprintf(value, "%16f", v);
}

void Options::Opt::operator=(double v)
{
    assert(sizeof(value) > 16);
    sprintf(value, "%16f", v);
}

Options::Options(const char* default_file_name)
{
    assert(default_file_name);

    if( 0 == inhome_path(default_filepath, max_path, default_file_name) )
    {
        memset(default_filepath, '\0', sizeof(default_filepath));
    }

#ifdef UNIX
    options[scrsvr_mode] = Opt( 'r', "--root",      "false",      "setup a root window for fullscreen mode"   );
    options[window_id]   = Opt( '-', "--window-id", "0",          "specific X window ID"                      );
    options[config]      = Opt( '-', "--config",    "0",          "show the configuration settings dialog box");
    options[device]      = Opt( '-', "--device",    "/dev/video0","setup a device for capturing video"        );
#endif//UNIX

#ifdef WIN32
    options[scrsvr_mode] = Opt( 's', "/s",          "false",      "start the screensaver in full-screen mode" );
    options[window_id]   = Opt( '-', "/p",          "0",          "preview the screensaver using the specified window handle" );
    options[config]      = Opt( '-', "/c",          "0",          "show the configuration settings dialog box");
    options[device]      = Opt( '-', "--device",    "",           "setup a device for capturing video"        );
#endif//WIN32

    options[width]       = Opt( '-', "--width",     "640",        "setup a width of screensaver window"       );
    options[height]      = Opt( '-', "--height",    "480",        "setup a height of screensaver window"      );
    options[vflip]       = Opt( 'V', "--vflip",     "false",      "vertical flip of a video frame"            );
    options[hflip]       = Opt( 'H', "--hflip",     "false",      "horisontal flip of a video frame"          );
    options[no_shaders]  = Opt( 'N', "--no-shaders","false",      "force disable shaders"                     );
    options[help]        = Opt( 'h', "--help",      "false",      "show this message and quit"                );
}

void Options::parse(int argc, char **argv)
{
    for(int i=1; i<argc; i++)
    {
        // is it begin of short key  string (-XXX) ?
        if( const char* sk_str = is_short_key(argv[i]) )
        {
            // loop over short key string
            for(int j=0; j<num && sk_str[j] != '\0'; j++)
            {
                int k = search_key(sk_str[j]);

                if(-1 == k) fprintf(stderr,"unknow short key: '%c'\n", sk_str[j]);
                else options[k] = "true";
            }
        }
        else
        {
            for(int j=0; j<num; j++)
            {
                if( strstr(argv[i], options[j].name) )
                {
                    if( '-' != options[j].key )     // is short key -X ?
                    {
                        options[j] = "true";
                    }
                    else if( char* s=strchr(argv[i],'=') ) // format: [key=value] ?
                    {
                        options[j] = s+1;
                    }
                    else if( char* s=strchr(argv[i],':') ) // format: [key:value] ?
                    {
                        options[j] = s+1;
                    }
                    else if( i+1 < argc )   // format: [key value] ?
                    {
                        //have we least one additional arg ?
                        // if it is - check it !
                        bool next_is_key = (search_name(argv[i+1]) != -1);

                        // is it begin of short key string (-XXX) ?
                        if( const char* sk_str = is_short_key(argv[i+1]) )
                        {
                            next_is_key = (search_key(*sk_str) != -1);
                        }

                        if(next_is_key)
                        {
                            fprintf(stderr, "'%s' key must have value! "
                                   "use syntax: [key=value], [key:value] "
                                   "or [key value]\n", argv[i]);
                        }
                        else options[j] = argv[++i];
                    }
                    break;
                }
            }
        }
    }
}


const Options::opt_name serializable[]={
    Options::device,
    Options::no_shaders,
    Options::vflip,
    Options::hflip,
    };

bool Options::save(const char* filepath)
{
    if(NULL == filepath) filepath = default_filepath; // use default filepath

    std::ofstream out(filepath, std::ios_base::binary | std::ios_base::out);
    if( !out.is_open() ) return false;

    const int count = sizeof(serializable)/sizeof(serializable[0]);
    for(int i=0; i<count; i++)
    {
        Options::Opt& opt = options[serializable[i]];
        if( !opt.empty() )
        {
            out << opt.name << '=' << opt.value << '\n';
        }
    }

    return out.good();
}

bool Options::load(const char* filepath)
{
    if(NULL == filepath) filepath = default_filepath; // use default filepath

    std::ifstream in(filepath, std::ios_base::binary | std::ios_base::in);
    if( !in.is_open() ) return false;

    const int count = sizeof(serializable)/sizeof(serializable[0]);

    char buffer[128];
    while( in.getline(buffer, sizeof(buffer)) )
    {
        for(int i=0; i<count; i++)
        {
            Options::Opt& opt = options[serializable[i]];

            if( strstr(buffer, opt.name) )
            {
                if( char* s=strchr(buffer,'=') ) // format: [key=value] ?
                {
                    opt = s+1;  // save new value
                }
                break;
            }
        }
    }
    return true;
}

int Options::usage()const
{
    fprintf(stdout, "possible arguments:\n");
    for(int i=0; i<num; i++)
    {
        if(options[i].key != '-')
            printf("  -%c ", options[i].key);
        else
            printf("     ");

        printf("%-12s value:%-12s\t%s\n", options[i].name, options[i].value, options[i].comment);
    }
    return 0;
}

int Options::search_name(const char* name)
{
    if( 0 < strcmp(name, "--") )
    for(int i=0; i<num; i++)
    {
        if( 0 == strcmp(name, options[i].name) )
            return i;
    }
    return -1;
}

int Options::search_key(char key)
{
    for(int i=0; i<num; i++)
    {
        if(key == options[i].key) return i;
    }
    return -1;
}

const char* Options::is_short_key(const char* str)
{
    if( '-' == str[0] && isalpha(str[1]) )
        return str+1;
    return NULL;
}
//-----------------------------------------------------------------------------
