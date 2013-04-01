//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   capture.cpp
// copyright:   (C) 2008, 2009, 2013 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cassert>
#include <memory>
#include <iostream>

#include <windows.h>
#include <atlbase.h>
#include <Mfapi.h>
#include <Mfidl.h>
#include <mferror.h>
#include <Mfreadwrite.h>
#include <Mftransform.h>


#include "capture.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void print(const GUID& guid, unsigned int width, unsigned int height, float rate)
{
    //Table of GUIDs http://msdn.microsoft.com/en-us/library/windows/desktop/aa370819(v=vs.85).aspx
    struct { DWORD dword; char c; } fourcc = {guid.Data1, '\0'};

    printf("%s %4ux%-4u %.1f fps\n", &fourcc, width, height, rate);
}

class Sampler: public IMFSourceReaderCallback
{
public:
    Sampler(Capture* c): cp(c),ref_count(0) {}
    ~Sampler(){}

    // IUnknown
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        static const QITAB qit[] =
        {
            QITABENT(Sampler, IMFSourceReaderCallback),
            { 0 },
        };
        return QISearch(this, qit, iid, ppv);
    }
    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&ref_count);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        ULONG count = InterlockedDecrement(&ref_count);
        if (count == 0)
        {
            delete this;
        }
        return count;
    }

    // IMFSourceReaderCallback
    STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*) { return S_OK; }
    STDMETHODIMP OnFlush(DWORD) { return S_OK; }
    STDMETHODIMP OnReadSample(
        HRESULT hrStatus,
        DWORD dwStreamIndex,
        DWORD dwStreamFlags,
        LONGLONG llTimestamp,
        IMFSample *pSample)
    {
    /*
        if (!IsCapturing())
        {
            LeaveCriticalSection(&m_critsec);
            return S_OK;
        }*/

        HRESULT hr = S_OK;

        if (pSample)
        {
            DWORD count = 0;
            pSample->GetBufferCount(&count);
            for(DWORD i=0; i<count; i++)
            {
                CComPtr<IMFMediaBuffer> buffer;
                pSample->GetBufferByIndex(i, &buffer);


                BYTE *buf;
                DWORD length;
                if (SUCCEEDED(buffer->Lock(&buf, NULL,&length)))
                {
                    cp->decode_to_buffer(buf, length);

                    buffer->Unlock();
                }
            }
        }

        if(SUCCEEDED(hrStatus))
        {
            hr = cp->reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                        0,
                                        NULL,   // actual
                                        NULL,   // flags
                                        NULL,   // timestamp
                                        NULL    // sample
                                        );
        }

        return hr;
    }

private:
    long ref_count;
    Capture* cp;
};

//-------------------------------------------------------------------
Capture::Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(FAILED(hr))
    {
        throw runtime_error("CoInitializeEx == failed");
    }

    hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);

    try
    {
        // find the device and init device pointer
        CComPtr<IMFActivate> device = find_device(dev_name);
        if(!device)
        {
            throw runtime_error("Video device isn't found");
        }

        // Create the media source for the device
        CComPtr<IMFMediaSource> source;
        hr = device->ActivateObject(__uuidof(IMFMediaSource), (void**)&source);
        {
            CComPtr<IMFAttributes> attributes;

            hr = MFCreateAttributes(&attributes, 1);

            if (SUCCEEDED(hr))
            {
                hr = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, new Sampler(this));
            }

            if (SUCCEEDED(hr))
            {
                hr = MFCreateSourceReaderFromMediaSource(source, attributes, &reader);
            }

            CComPtr<IMFMediaType> selected;

            int i=0;
            while(hr != MF_E_NO_MORE_TYPES)
            {
                CComPtr<IMFMediaType> type;
                hr = reader->GetNativeMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, i, &type);
                if (SUCCEEDED(hr))
                {
                    GUID subtype;
                    type->GetGUID(MF_MT_SUBTYPE, &subtype);
    /*
                    UINT32 mode;
                    type->GetUINT32(MF_MT_INTERLACE_MODE, &mode);

                    UINT32 bitrate;
                    type->GetUINT32(MF_MT_AVG_BITRATE, &bitrate);


                    UINT32 fcc;
                    type->GetUINT32(MF_MT_ORIGINAL_4CC, &fcc);
                

                    UINT32 yuv_matrix = 0;
                    type->GetUINT32(MF_MT_YUV_MATRIX, &yuv_matrix);
                
                    std::cout << mode << ' ' << yuv_matrix << ' ';*/


                    UINT32 width, height;
                    MFGetAttributeSize(type, MF_MT_FRAME_SIZE, &width, &height);

                    UINT32 numerator, denominator;
                    MFGetAttributeRatio(type, MF_MT_FRAME_RATE, &numerator, &denominator);
                    const float rate = float(numerator)/float(denominator);

                    if( (covet_w == width && covet_h == height) && rate >=30.0 )
                    {
                        if(BaseCapture::is_supported(subtype.Data1))
                        {
                            fourcc = subtype.Data1;
                            selected = type;

                            w = width;
                            h = height;

                            print(subtype, width, height, rate);
                            break;
                        }
                    }

                    i++;
                }
            }


            hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, selected);
            if (SUCCEEDED(hr))
            {
     //           std::cout << "Ok!!";
            }
        }

        /*
        CComPtr<IMFAttributes> attributes;

        hr = MFCreateAttributes(&attributes, 1);

        CComPtr<IPropertyStore> props;

        if (SUCCEEDED(hr))
        {
            hr = attributes->GetUnknown(MF_SOURCE_READER_MEDIASOURCE_CONFIG, __uuidof(props), (LPVOID*)&props);
        }*/

     /*
        hr = graph_builder->RenderStream(pin_category, &MEDIATYPE_Video, capture_filter, NULL, sampler);
        if (FAILED(hr))
        {
            throw runtime_error("Can not render the video capture stream hr=0x%x", hr);
        }*/
    }
    catch(runtime_error&)
    {
        CoUninitialize();
        throw;
    }
}

Capture::~Capture()
{
    MFShutdown();
    CoUninitialize();
}

bool Capture::set_buffer(unsigned char* buf, out_format fmt)
{
    CriticalSection::Lock lock(cs);

    HRESULT hr = S_OK;

    encoder tmp=NULL;
    if(buf != NULL)
    {
        tmp = get_encoder(fourcc, fmt);
        if(tmp == NULL) return false;

        hr = reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                        0,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);
    }
    else
    {
        hr = reader->Flush(MF_SOURCE_READER_FIRST_VIDEO_STREAM);
    }

    coder = tmp;
    format = fmt;
    buffer = buf;

    return SUCCEEDED(hr);
}

void Capture::decode_to_buffer(unsigned char* src, unsigned int length)
{
    CriticalSection::Lock lock(cs);

    if(buffer && coder)
    {
        (*coder)(src, src+length, buffer);
    }
}

IMFActivate* Capture::find_device(const char* dev_name)
{
    IMFActivate* device = NULL;
    HRESULT hr = S_OK;
    UINT32 number = 0;

    CComHeapPtr<IMFActivate*> devices;
    {
        CComPtr<IMFAttributes> attributes;
        hr = MFCreateAttributes(&attributes, 1);

        if(SUCCEEDED(hr))
        {
            hr = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                                        MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        }
        if(SUCCEEDED(hr))
        {
            hr = MFEnumDeviceSources(attributes, &devices, &number);
        }
    }

    const size_t size = strlen(dev_name)+1;
    std::unique_ptr<char[]> tmp(new char[size]);

    for(UINT32 i=0; i<number; i++)
    {
        CComHeapPtr<WCHAR> name;
        hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &name, NULL);
        if(SUCCEEDED(hr))
        {
            wcstombs_s(NULL, tmp.get(), size, name, _TRUNCATE);
        }

        if(SUCCEEDED(hr) && 0 == strncmp(tmp.get(), dev_name, size))
        {
            device = devices[i];
        }
        else
        {
            devices[i]->Release();
        }
    }
    return device;
}

unsigned int Capture::enum_devices(char buffers[][128], const unsigned int size)throw()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(FAILED(hr)) return 0;

    UINT32 number = 0;
    CComHeapPtr<IMFActivate*> devices;
    {
        CComPtr<IMFAttributes> attributes;
        hr = MFCreateAttributes(&attributes, 1);

        if(SUCCEEDED(hr))
        {
            hr = attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                                     MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        }
        if (SUCCEEDED(hr))
        {
            hr = MFEnumDeviceSources(attributes, &devices, &number);
        }
    }

    const unsigned int found_devices = min(size, number);

    for(unsigned int i=0; i<found_devices; i++)
    {
        CComHeapPtr<WCHAR> name;
        hr = devices[i]->GetAllocatedString(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, &name, NULL);
        if (SUCCEEDED(hr))
        {
            wcstombs_s(NULL, buffers[i], sizeof(buffers[i]), name, _TRUNCATE);
        }
    }

    for(UINT32 i = 0; i<number; i++)
    {
        devices[i]->Release();
    }

    CoUninitialize();

    return found_devices;
}
//-----------------------------------------------------------------------------
