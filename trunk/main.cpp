//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   main.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdio>

#include "application.h"
#include "app_window.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    Options opts(".mrainrc");
    opts.load();
    opts.parse(argc, argv);

    if( opts[Options::help] ) return opts.usage();

    // A little converter bmp -> *.h
    if( opts[Options::convert] )
    {
        return convert_bmp_2_include_array(argv[2], argv[3]) == false;
    }

    if( int id = opts[Options::config] )
    {
        Config config(opts);
        return config.show_dialog(id);
    }

    Application application(opts);

    return application.run();
}
//-----------------------------------------------------------------------------
