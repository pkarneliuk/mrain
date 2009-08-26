//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   capture.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cassert>

#include <AtlBase.h>

#include "capture.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
EXTERN_C const CLSID CLSID_SampleGrabber;
//-----------------------------------------------------------------------------
void printf(const GUID& guid, unsigned int width, unsigned int height);

Capture::Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name):media_control(NULL),filter_graph(NULL),graph_builder(NULL), stream_config(NULL), ref_count(1)
{
    const GUID* pin_category = &PIN_CATEGORY_CAPTURE;

    ZeroMemory(&media_type, sizeof(media_type));
    ZeroMemory(&video_info, sizeof(video_info));

    ICreateDevEnum* device_enum = NULL;
    IEnumMoniker*   enumerator  = NULL;

    try
    {
        // Initialize COM
        if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
        {
            throw runtime_error("CoInitializeEx == failed");
        }

        HRESULT hr = MB_OK;
        IBaseFilter *bf_capture_filter=NULL;

        // Create the system device enumerator
        hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (LPVOID*) &device_enum);
        if (FAILED(hr))
        {
            throw runtime_error("Couldn't create system device enumaration hr=0x%x", hr);
        }

        // Create an enumerator for the video capture devices
        hr = device_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumerator, 0);
        if( S_OK != hr )
        {
            throw runtime_error("Couldn't create class enumerator!  hr=0x%x", hr);
        }

        USES_CONVERSION;
        IMoniker* moniker = NULL;
        bool not_found = true;
        while( not_found && S_OK == enumerator->Next(1, &moniker, NULL) && moniker )
        {
            IPropertyBag* prop_bag = NULL;
            if(S_OK == moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)&prop_bag))
            {
                VARIANT name;
                VariantInit(&name);
                if(S_OK == prop_bag->Read(L"FriendlyName", &name, 0))
                {
                    if( 0 == strcmp(dev_name, OLE2A(name.bstrVal)) || '\0' == *dev_name ) // device is not set
                    {
                        printf("device:\t\t%s\n", dev_name);
                        hr = moniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&bf_capture_filter);
                        not_found = false;
                    }
                }
                VariantClear(&name);
            }
            prop_bag->Release();
            moniker->Release();
        }
        if(not_found || NULL == bf_capture_filter)
        {
            throw runtime_error("Couldn't bind moniker to filter object!  hr=0x%x", hr);
        }

        // Create the filter graph
        hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC, IID_IGraphBuilder, (LPVOID*) &filter_graph);
        if (FAILED(hr))
        {
            throw runtime_error("Can not create Filter Graph hr=0x%x", hr);
        }

        // Obtain interfaces for media control
        hr = filter_graph->QueryInterface(IID_IMediaControl,(LPVOID*) &media_control);
        if (FAILED(hr))
        {
            throw runtime_error("Can not get Media Control Interface hr=0x%x", hr);
        }

        // Create the capture graph builder
        hr = CoCreateInstance(CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC, IID_ICaptureGraphBuilder2, (LPVOID*) &graph_builder);
        if (FAILED(hr))
        {
            throw runtime_error("Can not create Capture Graph Builder 2 hr=0x%x", hr);
        }

        hr = graph_builder->SetFiltergraph(filter_graph);
        if (FAILED(hr))
        {
            throw runtime_error("Can not set Filter Graph hr=0x%x", hr);
        }

        hr = filter_graph->AddFilter(bf_capture_filter, L"Video Capture");
        if (FAILED(hr))
        {
            throw runtime_error("Can not add Video Capture filter hr=0x%x", hr);
        }

        hr = graph_builder->FindInterface(pin_category, 0, bf_capture_filter, IID_IAMStreamConfig, (void**)&stream_config);
        if (SUCCEEDED(hr))
        {
            int caps_count = 0;
            int size_of_caps = 0;
            hr = stream_config->GetNumberOfCapabilities(&caps_count, &size_of_caps);

            // Check the size to make sure we pass in the correct structure.
            if (size_of_caps == sizeof(VIDEO_STREAM_CONFIG_CAPS))
            {
                // Use the video capabilities structure.
                VIDEO_STREAM_CONFIG_CAPS scc={0};

                bool found = false;
                printf("supported formats:\n");
                for(int format=0; format < caps_count && !found; format++)
                {
                    AM_MEDIA_TYPE* mt = NULL;
                    hr = stream_config->GetStreamCaps(format, &mt, (BYTE*)&scc);
                    if (SUCCEEDED(hr))
                    {
                        if( mt->formattype == FORMAT_VideoInfo )
                        {
                            VIDEOINFOHEADER* video_info = (VIDEOINFOHEADER*)mt->pbFormat;
                            printf(mt->subtype, video_info->bmiHeader.biWidth, video_info->bmiHeader.biHeight);

                            if( mt->subtype == MEDIASUBTYPE_RGB24 ||
                                mt->subtype == MEDIASUBTYPE_RGB32 ||
                                mt->subtype == MEDIASUBTYPE_YUY2  ||
                                mt->subtype == MEDIASUBTYPE_YVYU )
                            {
                                // Set RGB24 format with any dimensions
                                stream_config->SetFormat(mt);

                                if( scc.MaxOutputSize.cx == int(covet_w) && scc.MaxOutputSize.cy == int(covet_h))
                                {
                                    found = true;
                                }
                            }
                        }

                        // delete the media type
                        CoTaskMemFree(mt->pbFormat);
                        CoTaskMemFree(mt);
                    }
                }
            }
        }


        IBaseFilter * bf_sample_grabber = NULL;
        hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&bf_sample_grabber);
        if (FAILED(hr))
        {
            throw runtime_error("Can not create SampleGrabber filter hr=0x%x", hr);
        }
        hr = filter_graph->AddFilter(bf_sample_grabber, L"Sample Grabber");
        if (FAILED(hr))
        {
            throw runtime_error("Can not add SampleGrabber filter to graph hr=0x%x", hr);
        }

        bf_sample_grabber->QueryInterface(IID_ISampleGrabber, (void**)&sample_grabber);
        
        
/*        media_type.majortype = MEDIATYPE_Video;
        media_type.subtype = MEDIASUBTYPE_RGB24;
        media_type.bFixedSizeSamples = TRUE;
        media_type.bTemporalCompression = FALSE;
        media_type.lSampleSize = 0;
        media_type.formattype = FORMAT_VideoInfo; // FORMAT_VideoInfo2;
        media_type.pUnk = 0; // Not used
        media_type.cbFormat = sizeof(video_info);
        media_type.pbFormat = reinterpret_cast<BYTE*>(&video_info);

        video_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        video_info.bmiHeader.biWidth = 160;
        video_info.bmiHeader.biHeight = 120;
        video_info.bmiHeader.biPlanes = 1;
        video_info.bmiHeader.biBitCount = 24;
    
        hr = sample_grabber->SetMediaType(&media_type);
        if (FAILED(hr))
        {
            bf_capture_filter->Release();
            throw runtime_error("Can not set MediaType hr=0x%x", hr);
        }
*/
        hr = sample_grabber->SetBufferSamples(FALSE);
        if (FAILED(hr))
        {
            throw runtime_error("Can not set Buffer Samples to FALSE hr=0x%x", hr);
        }

        hr = sample_grabber->SetCallback(this, 0);  // 0 - filter calls the ISampleGrabberCB::SampleCB method
        if (FAILED(hr))
        {
            throw runtime_error("Can not set Callback hr=0x%x", hr);
        }

        IBaseFilter* bf_null_renderer = NULL;
        hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&bf_null_renderer);
        if (FAILED(hr))
        {
            throw runtime_error("Can not create NullRenderer filter hr=0x%x", hr);
        }

        hr = filter_graph->AddFilter(bf_null_renderer, L"Null Renderer");
        if (FAILED(hr))
        {
            throw runtime_error("Can not add NullRenderer filter to graph hr=0x%x", hr);
        }

        hr = graph_builder->RenderStream (pin_category, &MEDIATYPE_Video, bf_capture_filter, bf_sample_grabber, bf_null_renderer);
        if (FAILED(hr))
        {
            bf_capture_filter->Release();
            throw runtime_error("Can not render the video capture stream hr=0x%x", hr);
        }

        // Start previewing video data
        hr = media_control->Run();
        if (FAILED(hr))
        {
            throw runtime_error("Can not run the graph hr=0x%x", hr);
        }

        hr = sample_grabber->GetConnectedMediaType(&media_type);
        
        // copy and free video info
        if(media_type.pbFormat != NULL)
        {
            if((media_type.formattype == FORMAT_VideoInfo) &&
            (media_type.cbFormat == sizeof(video_info)))
            {
                // copy VIDEOINFOHEADER into own memory
                CopyMemory(&video_info, media_type.pbFormat, sizeof(video_info) );
                printf("Selected device:\n");
                printf(media_type.subtype, width(), height());
            }
            
            CoTaskMemFree(media_type.pbFormat);
            
            if((media_type.formattype == FORMAT_VideoInfo) &&
            (media_type.cbFormat == sizeof(video_info)))
            {
                media_type.pbFormat = reinterpret_cast<BYTE*>(&video_info);
            }
            else
            {
                media_type.cbFormat = 0;
                media_type.pbFormat = NULL;
                throw runtime_error("Bad AM_MEDIA_TYPE");
            }
        }

        if(device_enum) device_enum->Release();
        if(enumerator) enumerator->Release();
    }
    catch(runtime_error&)
    {
        if (media_control) media_control->Release();
        if (graph_builder) graph_builder->Release();
        if (filter_graph ) filter_graph ->Release();
        if (stream_config) stream_config->Release();

        if(device_enum) device_enum->Release();
        if(enumerator) enumerator->Release();

        // clean all
        CoUninitialize();
        throw;
    }
}

Capture::~Capture()
{
    if (media_control) media_control->Stop(); // StopWhenReady();

    if (media_control) media_control->Release();
    if (graph_builder) graph_builder->Release();
    if (filter_graph ) filter_graph ->Release();
    if (stream_config) stream_config->Release();

    CoUninitialize();
}

STDMETHODIMP Capture::QueryInterface(REFIID riid, void** ppv)
{
    if(ppv == NULL) return E_INVALIDARG;
    if(riid == IID_IUnknown || riid == IID_ISampleGrabberCB)
    {
        *ppv = (IUnknown *)this;
        AddRef();
        return S_OK;
    }
    *ppv = NULL;
    return E_NOINTERFACE;
}

STDMETHODIMP Capture::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
    return E_NOTIMPL;
}

STDMETHODIMP Capture::SampleCB(double SampleTime, IMediaSample* sample)
{
    if(NULL == sample) return E_POINTER;

    if( buffer )
    {
        if(S_OK == sample->IsPreroll()) return S_OK;
        
        LONG data_len = sample->GetActualDataLength();

        BYTE* ptr = NULL;
        if((S_OK != sample->GetPointer(&ptr)) || !ptr) return E_FAIL;

        BYTE* end = ptr + data_len;

        const unsigned int frame_size = width() * height();
        unsigned int i = 0;

        if(media_type.subtype == MEDIASUBTYPE_RGB32)
        {
            for(; ptr<end && i<frame_size; ptr+=4, i++)
            {
                buffer[i] = *ptr;
            }
        }
        else if(media_type.subtype == MEDIASUBTYPE_RGB24)
        {
            for(; ptr<end && i<frame_size; ptr+=3, i++)
            {
                buffer[i] = *ptr;
            }
        }
        else if(media_type.subtype == MEDIASUBTYPE_YUY2 || 
        media_type.subtype == MEDIASUBTYPE_YVYU )
        {
            for(; ptr<end && i<frame_size; ptr+=4, i+=2)
            {
                buffer[i] = ptr[0];
                buffer[i+1]= ptr[2];
            }
        }
    }
    return S_OK;
}

const char* Capture::capture()
{
/*  Using internal buffers, low performance :(
    if( buffer )
    {
        // Find the required buffer size.
        long buffer_size = 0;
        HRESULT hr = sample_grabber->GetCurrentBuffer(&buffer_size, NULL);
        if(SUCCEEDED(hr) && buffer_size > 0)
        {
            long* buf = new long[buffer_size/sizeof(long)];
            if(buf)
            {
                if(S_OK == sample_grabber->GetCurrentBuffer(&buffer_size, buf))
                {
                    char* p = (char*)buf;
                    char* end = p + buffer_size;
                    unsigned int i = 0;
                    for(char* p = (char*)buf; p<end; p+=3)
                    {
                        buffer[i] = *p;
                        ++i;
                    }
                }
            }
            delete[] buf;
        }
    }
    */

    return buffer;
}

unsigned int Capture::enum_devices(char buffers[][128], unsigned int num)throw()
{
    ICreateDevEnum* device_enum = NULL;
    IEnumMoniker*   enumerator  = NULL;

    if(FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
        return 0;
    }

    // Create the system device enumerator
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC, IID_ICreateDevEnum, (LPVOID*) &device_enum);
    if (FAILED(hr))
    {
        CoUninitialize();
        return 0;
    }

    // Create an enumerator for the video capture devices
    if( S_OK != device_enum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumerator, 0) )
    {
        device_enum->Release();
        CoUninitialize();
        return 0;
    }

    IMoniker** monikers = new IMoniker*[num];
    memset(monikers, 0, num * sizeof(IMoniker*));
    
    ULONG fetched = 0;
    unsigned int found_devices = 0;
    hr = enumerator->Next(num, monikers, &fetched);
    if( fetched <= num )
    {
        USES_CONVERSION;
        for(ULONG i=0; i<fetched; i++)
        {
            assert(found_devices < fetched);
            IPropertyBag* prop_bag = NULL;
            if(S_OK == monikers[i]->BindToStorage(0, 0, IID_IPropertyBag, (void**)&prop_bag))
            {
                VARIANT name;
                VariantInit(&name);
                if(S_OK == prop_bag->Read(L"FriendlyName", &name, 0))
                {
                    assert(buffers[found_devices]);
                    strcpy_s(buffers[found_devices], sizeof(buffers[0]), OLE2A(name.bstrVal));
                    ++found_devices;
                }
                VariantClear(&name);
            }
            if(prop_bag) prop_bag->Release();
            monikers[i]->Release();
        }
    }

    delete[] monikers;

    if(enumerator ) enumerator ->Release();
    if(device_enum) device_enum->Release();

    CoUninitialize();
    return found_devices;
}

void printf(const GUID& guid, unsigned int width, unsigned int height)
{
    unsigned char fourcc[8]={'\0'};

    // Analyze FourCC field
    //ms-help://MS.MSDNQTR.v90.en/directshow/htm/howtoregisterafourcc.htm
    if(guid == MEDIASUBTYPE_RGB24) // it is not mnemonic
    {
        fourcc[0] = 'R';
        fourcc[1] = 'G';
        fourcc[2] = 'B';
        fourcc[3] = '2';
        fourcc[4] = '4';
    }
    else if(guid == MEDIASUBTYPE_RGB32) // it is not mnemonic
    {
        fourcc[0] = 'R';
        fourcc[1] = 'G';
        fourcc[2] = 'B';
        fourcc[3] = '3';
        fourcc[4] = '2';
    }
    else
    {
        fourcc[0] = (guid.Data1>>0 ) & 0xFF;
        fourcc[1] = (guid.Data1>>8 ) & 0xFF;
        fourcc[2] = (guid.Data1>>16) & 0xFF;
        fourcc[3] = (guid.Data1>>24) & 0xFF;
    }
    printf("'%s'\t %ix%i\n", fourcc, width, height);
}
//-----------------------------------------------------------------------------
