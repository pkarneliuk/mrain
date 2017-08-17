//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "stuff.h"
#include <windows.h>
//------------------------------------------------------------------------------
class NativeWindow : noncopyable
{
    friend class GLContext;

protected:
    NativeWindow();
    ~NativeWindow();

    void create_window(class AppWindow* win, unsigned int parent_id, int width,
                       int height);
    void activate();
    bool process_events();

private:
    enum
    {
        MAX_EVENTS_PROCESSING_TIME = 30,
        BREAK_TIMER_ID
    };

    HWND  hwnd;
    POINT start_point;

    static LRESULT ScreenSaverProc(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam);
};

class Config : noncopyable
{
public:
    Config(class Options& opts);

    bool show_dialog(int parent_id);

private:
    static INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam);

    Options& options;
};
//------------------------------------------------------------------------------
