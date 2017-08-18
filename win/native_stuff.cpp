//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "native_stuff.h"
#include <cstdio>
#include <shlobj.h>
//------------------------------------------------------------------------------

unsigned int make_seed()
{
    LARGE_INTEGER tp;
    QueryPerformanceCounter(&tp);
    return (999 * tp.LowPart) + (1001 * tp.HighPart) +
           (1003 * GetCurrentProcessId());
}

unsigned int inhome_path(char* buffer, size_t size, const char* file_name)
{
    char tmp_path[MAX_PATH];
    if(S_FALSE == SHGetFolderPathA(NULL,
                                   CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
                                   NULL, SHGFP_TYPE_CURRENT, tmp_path))
        return 0;
    unsigned int len = _snprintf(buffer, size, "%s\\%s", tmp_path, file_name);

    return len < size ? len : 0;
}

static class WinConsole
{
public:
    WinConsole()
    {
        // Redirect IO to console if it exist
        BOOL res = AttachConsole(ATTACH_PARENT_PROCESS);
#ifdef _DEBUG
        if(res == FALSE)
            res = AllocConsole();
#endif//_DEBUG
        if(res != FALSE)
        {
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
            freopen("CONIN$", "r", stdin);
        }
    }

    ~WinConsole() { FreeConsole(); }
} console;// initialization stdin and etc in static object may be dangerous
//------------------------------------------------------------------------------
