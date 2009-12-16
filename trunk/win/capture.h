//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   capture.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef CAPTURE_H
#define CAPTURE_H
//-----------------------------------------------------------------------------
#include <windows.h>
#include <AtlBase.h>
#include <Dshow.h>
#include <Qedit.h>

#include "base_capture.h"
//-----------------------------------------------------------------------------
class Capture:public BaseCapture, public ISampleGrabberCB
{
public:
    Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name);
    ~Capture();

    // IUnknown
    STDMETHODIMP    QueryInterface (REFIID iid, void **ppv);
    STDMETHODIMP_(ULONG) AddRef(void)  { return InterlockedIncrement(&ref_count); }
    STDMETHODIMP_(ULONG) Release(void) { return InterlockedDecrement(&ref_count); }

    // ISampleGrabberCB
    STDMETHODIMP    BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen);
    STDMETHODIMP    SampleCB(double SampleTime, IMediaSample *pSample);


    const char* capture();
    unsigned int width () { return video_info.bmiHeader.biWidth;  }
    unsigned int height() { return video_info.bmiHeader.biHeight; }

    static unsigned int enum_devices(char buffers[][128], unsigned int num)throw();

private:

    CComPtr<IMediaControl>           media_control;
    CComPtr<IGraphBuilder>           filter_graph;
    CComPtr<ICaptureGraphBuilder2>   graph_builder;
    CComPtr<ISampleGrabber>          sample_grabber;
    CComPtr<IAMStreamConfig>         stream_config;

    AM_MEDIA_TYPE media_type;
    VIDEOINFOHEADER video_info;

    LONG ref_count;
};
//-----------------------------------------------------------------------------
#endif//CAPTURE_H
//-----------------------------------------------------------------------------
