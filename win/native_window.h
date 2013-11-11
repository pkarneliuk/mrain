//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   native_window.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef NATIVE_WINDOW_H
#define NATIVE_WINDOW_H
//-----------------------------------------------------------------------------
#include <windows.h>

#include "resource.h"   // windows only
#include "base_window.h"
#include "options.h"
//-----------------------------------------------------------------------------
class NativeWindow:protected BaseWindow
{
    friend class GLContext;
public:
    NativeWindow(BaseWindow::Mode mode, unsigned int parent_id, int width, int height);
    ~NativeWindow();

    bool process_events();
    void get_size(unsigned int* width, unsigned int* height)const;

private:

    enum{ MAX_EVENTS_PROCESSING_TIME=30, BREAK_TIMER_ID };

    HWND hwnd;
    POINT start_point;

    static const char* win_class_name;

    static LRESULT ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

class Config
{
public:
    Config(Options& opts);
    Config& operator=(Config&);  // undefined

    bool show_dialog(int parent_id);

private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    Options&  options;
    bool registration;
};
//-----------------------------------------------------------------------------
#endif//NATIVE_WINDOW_H
//-----------------------------------------------------------------------------
