//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_stuff.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cstdio>

#include <shlobj.h>

#include "native_stuff.h"
//-----------------------------------------------------------------------------
LARGE_INTEGER Timer::frequency={0L};
//-----------------------------------------------------------------------------
Timer::Timer()
{
    if(0L == Timer::frequency.QuadPart)
    {
        QueryPerformanceFrequency(&Timer::frequency);
        Timer::frequency.QuadPart /= 1000000; //microseconds
    }
    reset();
}

void Timer::reset()
{
    QueryPerformanceCounter(&begin);
    prev = begin;
}

unsigned long Timer::time()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    return unsigned long((now.QuadPart - begin.QuadPart)/frequency.QuadPart);
}

unsigned long Timer::tick()
{
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);

    unsigned long delta = unsigned long((now.QuadPart - prev.QuadPart)/Timer::frequency.QuadPart);
    prev = now;

    return delta;
}

//-----------------------------------------------------------------------------

Timer Waiter::timer;

Waiter::Waiter(unsigned long microseconds):sec(microseconds / 1000000), usec(microseconds % 1000000)
{
    end.QuadPart = timer.prev.QuadPart + (sec * 1000000 + usec) * Timer::frequency.QuadPart;
}

bool Waiter::ready()
{
    bool done = false;
    if(end.QuadPart < timer.prev.QuadPart)
    {
        done = true;
    }

    if( done )
    {
        end.QuadPart += (sec * 1000000 + usec) * Timer::frequency.QuadPart;
    }
    return done;
}

void Waiter::tick()
{
    timer.tick();
}

//-----------------------------------------------------------------------------

unsigned int make_seed()
{
    LARGE_INTEGER tp;
    QueryPerformanceCounter(&tp);
    return (999*tp.LowPart) + (1001*tp.HighPart) + (1003 * GetCurrentProcessId());
}

unsigned int inhome_path(char* buffer, size_t size, const char* file_name)
{
    char tmp_path[MAX_PATH];
    if( S_FALSE == SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE , NULL, SHGFP_TYPE_CURRENT, tmp_path) )
        return 0;
    unsigned int len = _snprintf(buffer, size, "%s\\%s", tmp_path, file_name);

    return len < size ? len : 0;
}

int main(int argc, char **argv); // forvard declaration

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    // Redirect IO to console if it exist
    if( AttachConsole(ATTACH_PARENT_PROCESS) == TRUE )
    {
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$",  "r", stdin );
    }

    char* argv[16]={NULL};
    int argc = 0;

    {
        // Parse commandline
        // For example: app.exe --abc abc ""   "abc abc"   "abc" " a b c "   abc="trhrthrthe erge "
        // Fill argc and argv
        char* cmd = GetCommandLine();

        // My little state-machine
        enum state{SPACE, STRING, DQUOTES};
        state s = SPACE;

        while(*cmd && argc < sizeof(argv)/sizeof(argv[0]))
        {
            switch(s)
            {
                case SPACE:
                    if(isspace(*cmd)) *cmd = '\0';
                    else
                    {
                        argv[argc++] = cmd;
                        if(*cmd == '"')
                        {
                            cmd[0] = cmd[1];   // shift one left
                            s = DQUOTES;
                        }
                        else
                        {
                            s = STRING;
                        }
                    }
                break;

                case STRING:
                    if(isspace(*cmd))
                    {
                        *cmd = '\0';
                        s = SPACE;
                    }
                    else if(*cmd == '"')
                    {
                        cmd[0] = cmd[1];   // shift one left
                        s = DQUOTES;
                    }
                break;

                case DQUOTES:
                    if(*cmd == '"')
                    {
                        *cmd = '\0';
                        if(cmd[-1] == '"') cmd[-1] = '\0';
                        s = SPACE;
                    }
                    else cmd[0] = cmd[1]; // shift one left
                break;
            };
            cmd++;
        }
    }

    int ret = main(argc, argv);
    FreeConsole();
    return ret;
}
//-----------------------------------------------------------------------------
