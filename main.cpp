//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "app_window.h"
#include "application.h"
//------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    Options opts(".mrainrc");
    opts.load();
    opts.parse(argc, argv);

    if(opts[Options::help])
        return opts.usage();

    int id = opts[Options::config];
    if(id)
    {
        Config config(opts);
        return config.show_dialog(0);
    }

    Application application(opts);

    return application.run();
}
//------------------------------------------------------------------------------
