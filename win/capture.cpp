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
void print(const GUID& guid, unsigned int width, unsigned int height, unsigned int rate)
{
    //Table of GUIDs http://msdn.microsoft.com/en-us/library/windows/desktop/aa370819(v=vs.85).aspx
    struct { DWORD dword; char c; } fourcc = {guid.Data1, '\0'};

    printf("%s %4ux%-4u %3u fps\n", &fourcc, width, height, rate);
}

class Sampler: public IMFSourceReaderCallback
{
public:
    Sampler(const Capture& c): cp(c), ref_count(0) {}
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
        DWORD,
        DWORD,
        LONGLONG,
        IMFSample *pSample)
    {
        HRESULT hr = S_OK;

        if(SUCCEEDED(hrStatus))
        {
            hr = cp.async_read();
        }

        if (pSample)
        {
            DWORD count = 0;
            pSample->GetBufferCount(&count);
            for(DWORD i=0; i<count; i++)
            {
                CComPtr<IMFMediaBuffer> buffer;
                if(FAILED(pSample->GetBufferByIndex(i, &buffer))) continue;

                CComPtr<IMF2DBuffer> buffer2D;
                buffer->QueryInterface<IMF2DBuffer>(&buffer2D);

                if(buffer2D)
                {
                    BYTE *scanline0;
                    LONG pitch;
                    if (SUCCEEDED(buffer2D->Lock2D(&scanline0, &pitch)))
                    {
                        cp.decode_padded_to_buffer(scanline0, pitch);
                        buffer2D->Unlock2D();
                    }
                }
                else
                {
                    BYTE *buf;
                    DWORD length;
                    if (SUCCEEDED(buffer->Lock(&buf, NULL, &length)))
                    {
                        cp.decode_to_buffer(buf, length);
                        buffer->Unlock();
                    }
                }
            }
        }

        return hr;
    }

private:
    long ref_count;
    const Capture& cp;
};

//-------------------------------------------------------------------
Capture::Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name) try
    :stride(0)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if(FAILED(hr)) throw hr;

    hr = MFStartup(MF_VERSION, MFSTARTUP_LITE);

    // find the device and init device pointer
    CComPtr<IMFActivate> device = find_device(dev_name);
    if(!device)
    {
        throw runtime_error("Video device isn't found");
    }

    {   // create the media source for the device
        CComPtr<IMFMediaSource> source;
        hr = device->ActivateObject(__uuidof(IMFMediaSource), (void**)&source);
        if(FAILED(hr)) throw hr;

        {   // create source reader
            CComPtr<IMFSourceReaderCallback> sampler = new Sampler(*this);
            CComPtr<IMFAttributes> attributes;
            hr = MFCreateAttributes(&attributes, 1);
            if(FAILED(hr)) throw hr;
            hr = attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, sampler);
            if(FAILED(hr)) throw hr;
            hr = MFCreateSourceReaderFromMediaSource(source, attributes, &reader);
            if(FAILED(hr)) throw hr;
        }
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

            UINT32 tmp = 0;
            type->GetUINT32(MF_MT_DEFAULT_STRIDE, &tmp);
            stride = static_cast<INT32>(tmp);

            UINT32 width, height;
            MFGetAttributeSize(type, MF_MT_FRAME_SIZE, &width, &height);

            UINT32 numerator, denominator;
            MFGetAttributeRatio(type, MF_MT_FRAME_RATE, &numerator, &denominator);
            rate = numerator/denominator;

            if( (covet_w == width && covet_h == height) && rate >= 30 )
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

    if(!selected)
    {
        throw runtime_error("no one supported video format");
    }

    hr = reader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_VIDEO_STREAM, NULL, selected);
    if(FAILED(hr)) throw hr;
}
catch(HRESULT hr)
{
    TCHAR text[1024]=TEXT("Unknown video capture error.");

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    hr,
                    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
                    (LPTSTR)&text,
                    sizeof(text)/sizeof(text[0]),
                    NULL);

    throw runtime_error(text);
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
        tmp = get_transform(fourcc).func[fmt];
        if(tmp == NULL) return false;

        hr = async_read();
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

HRESULT Capture::async_read()const
{
    return reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM,
                                0,
                                NULL,
                                NULL,
                                NULL,
                                NULL);
}

void Capture::decode_to_buffer(unsigned char* src, unsigned int length)const
{
    CriticalSection::Lock lock(cs);

    if(buffer && coder)
    {
        (*coder)(src, src+length, buffer);
    }
}

void Capture::decode_padded_to_buffer(unsigned char* scanline0, unsigned int pitch)const
{
    CriticalSection::Lock lock(cs);

    if(buffer && coder)
    {
        unsigned char * src = scanline0;
        unsigned char * dst = buffer;
        size_t length = w * (BaseCapture::get_transform(fourcc)).bpp / 8;

        for(unsigned int y=0; y<h; y++)
        {
            (*coder)(src, src+length, dst);

            dst += w * bpp(format);
            src += pitch;
        }
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

        if(SUCCEEDED(hr) /*&& 0 == strncmp(tmp.get(), dev_name, size)*/)
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
