//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_window.h
// copyright:   (C) 2008, 2009, 2014 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef NATIVE_WINDOW_H
#define NATIVE_WINDOW_H
//-----------------------------------------------------------------------------
#include <windows.h>
//-----------------------------------------------------------------------------
class NativeWindow
{
    friend class GLContext;
protected:
    NativeWindow();
    ~NativeWindow();

    void create_window(class AppWindow* win, unsigned int parent_id, int width, int height);
    void activate();
    bool process_events();

private:
    enum{ MAX_EVENTS_PROCESSING_TIME=30, BREAK_TIMER_ID };

    HWND hwnd;
    POINT start_point;

    static LRESULT ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

class Config
{
public:
    Config(class Options& opts);
    Config& operator=(Config&);  // undefined

    bool show_dialog(int parent_id);

private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    Options& options;
};
//-----------------------------------------------------------------------------
#endif//NATIVE_WINDOW_H
//-----------------------------------------------------------------------------
