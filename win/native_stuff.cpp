//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#include "native_stuff.h"
#include <Shlobj.h>
#include <experimental/filesystem>
//------------------------------------------------------------------------------
namespace fs = std::experimental::filesystem;
fs::path inhome_path(const fs::path& file_name)
{
    PWSTR    str = nullptr;
    fs::path result;
    if(S_OK !=
       SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &str))
        return result;

    result = str;
    CoTaskMemFree(str);
    return result /= file_name;
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
