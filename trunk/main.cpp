//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   main.cpp
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "application.h"
#include "app_window.h"
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    Options opts(".mrainrc");
    opts.load();
    opts.parse(argc, argv);

    if( opts[Options::help] ) return opts.usage();

    if( int id = opts[Options::config] )
    {
        Config config(opts);
        return config.show_dialog(0);
    }

    Application application(opts);

    return application.run();
}
//-----------------------------------------------------------------------------
