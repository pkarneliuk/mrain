//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   capture.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include <unistd.h>
#include <memory.h>
#include <fcntl.h>       // low-level i/o
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev.h>

#include "capture.h"
#include "stuff.h"
//-----------------------------------------------------------------------------
// Capture implementation based on V4L2 documentation:
// "Video for Linux Two API Specification. Revision 0.24"
// http://www.linuxtv.org/downloads/video4linux/API/V4L2_API/v4l2spec/v4l2.pdf
//-----------------------------------------------------------------------------
#define CLEAR(x) memset (&(x), 0, sizeof (x))
static enum v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//-----------------------------------------------------------------------------
int xioctl(int fd, int request, void *arg)
{
    int r;
    do r = ioctl (fd, request, arg);
    while (-1 == r && EINTR == errno);
    return r;
}

Capture::Device::Device(const char* dev_name):fd(-1)
{
    struct stat st={0};
    if( -1 == stat(dev_name, &st) )
    {
        throw runtime_error("'%s' is not exist. errno:%i", dev_name, errno);
    }

    if( !S_ISCHR(st.st_mode) )
    {
        throw runtime_error("'%s' is not a character device.", dev_name);
    }

    if( 81 != major(st.st_rdev) ) // V4L2 device nodes must have 81 major number
    {
        throw runtime_error("major device ID of '%s' is not equal 81.", dev_name);
    }

    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);
    if (-1 == fd)
    {
        throw runtime_error("'%s' can not be oppened. errno:%i", dev_name, errno);
    }
}

Capture::Device::~Device()
{
    close(fd);
}

Capture::MMapBuffer::MMapBuffer(int fd, size_t len, off_t off):length(len)
{
    start = mmap (NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, off);
    if (MAP_FAILED == start)
        throw runtime_error("mmap failed");
}
Capture::MMapBuffer::~MMapBuffer()
{
    if (-1 == munmap (start, length))
        throw runtime_error("munmap failed");
}

Capture::Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name):
    device(dev_name), buffers(NULL), decoders(NULL)
{
    // obtain information about driver and hardware capabilities
    struct v4l2_capability cap;
    if(-1 == xioctl(device, VIDIOC_QUERYCAP, &cap))
    {
        if (EINVAL == errno)
        {
            throw runtime_error("'%s' is no V4L2 device", dev_name);
        }
    }

    // print device capcabilities
    fprintf(stdout,"device:\t\t%s [%s]\n", dev_name, cap.card);
    fprintf(stdout,"driver:\t\t%s\n", cap.driver);
    fprintf(stdout,"bus info:\t%s\n", cap.bus_info);
    char major = (cap.version>>16) & 0xFF;
    char minor = (cap.version>>8)  & 0xFF;
    char subminor = (cap.version)  & 0xFF;
    fprintf(stdout,"version:\t%d.%d.%d\n", major, minor, subminor);
    fprintf(stdout,"capabilities:\n");	

    if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
        fprintf(stdout, "\tVideo Capture interface\n");
    if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)
        fprintf(stdout, "\tVideo Output interface\n");
    if(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY)
        fprintf(stdout, "\tVideo Overlay interface\n");
    if(cap.capabilities & V4L2_CAP_VBI_CAPTURE)
        fprintf(stdout, "\tRaw VBI Capture interface\n");
    if(cap.capabilities & V4L2_CAP_VBI_OUTPUT)
        fprintf(stdout, "\tRaw VBI Output interface\n");
    if(cap.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE)
        fprintf(stdout, "\tSliced VBI Capture interface\n");
    if(cap.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT)
        fprintf(stdout, "\tSliced VBI Output interface\n");
    if(cap.capabilities & V4L2_CAP_RDS_CAPTURE)
        fprintf(stdout, "\tRDS data capture\n");
    if(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY)
        fprintf(stdout, "\tVideo Output Overlay(OSD) interface\n");

    if(cap.capabilities & V4L2_CAP_TUNER)
        fprintf(stdout, "\tSome sort of tuner to receive video signals\n");
    if(cap.capabilities & V4L2_CAP_AUDIO)
        fprintf(stdout, "\tThe device has audio inputs or outputs\n");
    if(cap.capabilities & V4L2_CAP_RADIO)
        fprintf(stdout, "\tThis is a radio receiver\n");

    if(cap.capabilities & V4L2_CAP_READWRITE)
        fprintf(stdout, "\tread/write I/O methods\n");
    if(cap.capabilities & V4L2_CAP_ASYNCIO)
        fprintf(stdout, "\tAsynchronous I/O methods\n");
    if(cap.capabilities & V4L2_CAP_STREAMING)
        fprintf(stdout, "\tStreaming I/O methods\n");

    // print predefined user controls
    struct v4l2_queryctrl queryctrl;
    CLEAR(queryctrl);

    fprintf(stdout, "user controls:\n");
    for(queryctrl.id = V4L2_CID_BASE; queryctrl.id < V4L2_CID_LASTP1; queryctrl.id++)
    {
        if(0 == xioctl(device, VIDIOC_QUERYCTRL, &queryctrl))
        {
            if(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) continue;
            fprintf(stdout, "\t%s\n", queryctrl.name);
            if (queryctrl.type == V4L2_CTRL_TYPE_MENU)
            {
                struct v4l2_querymenu querymenu;
                CLEAR(querymenu);
                querymenu.id = queryctrl.id;
                for(querymenu.index = queryctrl.minimum;
                    querymenu.index <= (unsigned int)queryctrl.maximum;
                    querymenu.index++)
                {
                    if(0 == xioctl(device, VIDIOC_QUERYMENU, &querymenu))
                    {
                        fprintf(stdout, "\t\titem: %s\n", querymenu.name);
                    } else break;
                }
            }
        }
        else
        {
            if(errno == EINVAL) continue;
            else break;
        }
    }

    // check video capture interface
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        throw runtime_error("'%s' does not support Video Capture interface", dev_name);
    }

    // check streaming i/o
    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        throw runtime_error("'%s' does not support Streaming I/O", dev_name);
    }

    // set video format
    struct v4l2_format fmt;
    CLEAR (fmt);

    fmt.type                = buf_type;
    fmt.fmt.pix.pixelformat = select_format();
    fmt.fmt.pix.bytesperline= 0;    // no padding
    fmt.fmt.pix.width       = covet_w;
    fmt.fmt.pix.height      = covet_h;

    // VIDIOC_S_FMT may change width and height.
    if( -1 == xioctl(device, VIDIOC_S_FMT, &fmt) )
    {
        throw runtime_error("VIDIOC_S_FMT = -1 errno:%i", errno);
    }
    else
    {
        fprintf(stdout, "w:%i h:%i sizeimage:%i\n",
                        fmt.fmt.pix.width, 
                        fmt.fmt.pix.height,
                        fmt.fmt.pix.sizeimage);

        captured_width = fmt.fmt.pix.width;
        captured_height= fmt.fmt.pix.height;

        if( (captured_width % 4) != 0)
        {
            throw runtime_error("captred frame width must be align of 4");
        }

        // set decoder
        for(unsigned int i=0; i<sizeof(supported_formats)/sizeof(supported_formats[0]); i++)
        {
            if(fmt.fmt.pix.pixelformat == supported_formats[i].fourcc)
            {
                decoders = supported_formats[i].decoder;
                break;
            }
        }
    }

/*    // Buggy driver paranoia.
    unsigned int min = 0;

    min = fmt.fmt.pix.width * 2;
    if(fmt.fmt.pix.bytesperline < min) fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if(fmt.fmt.pix.sizeimage < min) fmt.fmt.pix.sizeimage = min;
*/

    struct v4l2_requestbuffers req;
    CLEAR (req);

    req.count   = 2;
    req.type    = buf_type;
    req.memory  = V4L2_MEMORY_MMAP;

    if(-1 == xioctl(device, VIDIOC_REQBUFS, &req))
    {
        if(EINVAL == errno)
        {
            throw runtime_error("'%s' does not support memory mapping\n", dev_name);
        }
        else
        {
            throw runtime_error("VIDIOC_REQBUFS");
        }
    }

    if(req.count < 2)
    {
        throw runtime_error("Insufficient buffer memory on %s\n", dev_name);
    }
    num_buffers = req.count;
    buffers = new MMapBuffer*[num_buffers];

    for (unsigned int i=0; i<num_buffers; ++i)
    {
        struct v4l2_buffer buf;
        CLEAR (buf);

        buf.type        = buf_type;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = i;

        xioctl (device, VIDIOC_QUERYBUF, &buf);

        buffers[i] = new MMapBuffer(device, buf.length, buf.m.offset);
        xioctl (device, VIDIOC_QBUF, &buf);
    }

    xioctl (device, VIDIOC_STREAMON, &buf_type);
}

Capture::~Capture()
{
    xioctl(device, VIDIOC_STREAMOFF, &buf_type);

    for (unsigned int i=0; i<num_buffers; ++i)
        delete buffers[i];

    delete[] buffers;
}

const char* Capture::capture()
{
    if( buffer )  // output buffer must be set
    {
/*        fd_set fds;
        FD_ZERO (&fds); 
        FD_SET (device, &fds);

        timeval timeout={0, 5000};

        int ret = select (device+1, &fds, NULL, NULL, &timeout);

        // select timeout or something worse
        if(1 != ret) return NULL;
*/
        struct v4l2_buffer buf;
        CLEAR (buf);

        buf.type = buf_type;
        buf.memory = V4L2_MEMORY_MMAP;

        // dequeue a filled (capturing) buffer from the driver’s outgoing queue
        if(0 == xioctl(device, VIDIOC_DQBUF, &buf))
        {
            assert(buf.index < num_buffers);

            MMapBuffer* current = buffers[buf.index];
            decoders[format]((char*)current->start, (char*)current->start + current->length, buffer);

            // enqueue an empty (capturing) buffer in the driver’s incoming queue
            xioctl(device, VIDIOC_QBUF, &buf);
        }
    }
    return buffer;
}

unsigned int Capture::select_format()
{
    fprintf(stdout, "video formats:\n");
    errno = 0;

    unsigned int format = 0;
    for(unsigned int i=0; errno != EINVAL; i++)
    {
        v4l2_fmtdesc desc={0};
        desc.index = i;
        desc.type = buf_type;

        if (-1 != xioctl(device, VIDIOC_ENUM_FMT, &desc))
        {
            if(0U == format)    // find if not found
            {
                for(unsigned int j=0; j<sizeof(supported_formats)/sizeof(supported_formats[0]); j++)
                {
                    if(supported_formats[j].fourcc == desc.pixelformat )
                    {
                        format = desc.pixelformat;
                        break;
                    }
                }
            }
            char str[5]={'\0'};
            str[0] = (desc.pixelformat>>0 ) & 0xFF;
            str[1] = (desc.pixelformat>>8 ) & 0xFF;
            str[2] = (desc.pixelformat>>16) & 0xFF;
            str[3] = (desc.pixelformat>>24) & 0xFF;

            if(format != desc.pixelformat)
                fprintf(stdout, "\tFourCC[%s] %s\n", str, (const char*)desc.description);
            else
                fprintf(stdout, "select->FourCC[%s] %s\n", str, (const char*)desc.description);
        }
    }

    if(0 == format) fprintf(stderr, "not found supported formats, sorry\n");
    return format;
}

const Capture::Converter Capture::supported_formats[4]=
{
    { V4L2_PIX_FMT_YUYV, { YUYVtoRGB, YUYVtoBGR, YUYVtoGRAY } },
    { V4L2_PIX_FMT_UYVY, { UYVYtoRGB, UYVYtoBGR, UYVYtoGRAY } },
    { V4L2_PIX_FMT_GREY, { 0,         0,         GREYtoGREY } },
    { V4L2_PIX_FMT_YUV422P, { 0,      0,      YUV422PtoGREY } },
};

void Capture::GREYtoGREY(const char* src, const char* end, char* dst)
{
    memcpy(dst, src, end - src);
}

#define CLAMP(c) if(c & (~255)) {c = (unsigned char)~(c >> (sizeof(c)*8 -1 ));}

void Capture::YUYVtoRGB(const char* c, const char* e, char* dst)
{
    unsigned const char* src = (unsigned const char*)c;
    unsigned const char* end = (unsigned const char*)e;

    int r, g, b, cr, cg, cb, y1, y2;
    for(; src<end; src+=4)
    {
        y1 = src[0];
        cb = ((src[1] -128)*454) >> 8;
        cg = ((src[1] -128)*88);

        y2 = src[2];
        cr = ((src[3] - 128) * 359) >> 8;
        cg = (cg + (src[3] - 128) * 183) >> 8;

        r = y1 + cr;
        b = y1 + cb;
        g = y1 - cg;
        CLAMP(r);
        CLAMP(g);
        CLAMP(b);

        dst[0] = r;
        dst[1] = g;
        dst[2] = b;
        dst +=3;

        r = y2 + cr;
        b = y2 + cb;
        g = y2 - cg;
        CLAMP(r);
        CLAMP(g);
        CLAMP(b);

        dst[0] = r;
        dst[1] = g;
        dst[2] = b;
        dst +=3;
    }
}

void Capture::YUYVtoBGR(const char* c, const char* e, char* dst)
{
    unsigned const char* src = (unsigned const char*)c;
    unsigned const char* end = (unsigned const char*)e;

    int r, g, b, cr, cg, cb, y1, y2;
    for(; src<end; src+=4)
    {
        y1 = src[0];
        cb = ((src[1] -128)*454) >> 8;
        cg = ((src[1] -128)*88);

        y2 = src[2];
        cr = ((src[3] - 128) * 359) >> 8;
        cg = (cg + (src[3] - 128) * 183) >> 8;

        r = y1 + cr;
        b = y1 + cb;
        g = y1 - cg;
        CLAMP(r);
        CLAMP(g);
        CLAMP(b);

        dst[0] = b;
        dst[1] = g;
        dst[2] = r;
        dst +=3;

        r = y2 + cr;
        b = y2 + cb;
        g = y2 - cg;
        CLAMP(r);
        CLAMP(g);
        CLAMP(b);

        dst[0] = b;
        dst[1] = g;
        dst[2] = r;
        dst +=3;
    }
}

void Capture::YUYVtoGRAY(const char* src, const char* end, char* dst)
{
    for(; src<end; src+=4)
    {
        *dst++ = src[0];
        *dst++ = src[2];
    }
}

void Capture::UYVYtoRGB(const char* src, const char* end, char* dst)
{
    for(; src<end; src+=4) 
    {
        char u  = src[0];
        char y1 = src[1];
        char v  = src[2];
        char y2 = src[3];

        float r = + 2.032f*float(u);
        float g = - 0.395f*float(u) -0.581f*float(v);
        float b = + 1.140f*float(v);

        dst[0] = float(y1) + r;
        dst[1] = float(y1) + g;
        dst[2] = float(y1) + b;

        dst +=3;

        dst[0] = float(y2) + r;
        dst[1] = float(y2) + g;
        dst[2] = float(y2) + b;

        dst +=3;
    }
}

void Capture::UYVYtoBGR(const char* src, const char* end, char* dst)
{
    for(; src<end; src+=4)
    {
        char u  = src[0];
        char y1 = src[1];
        char v  = src[2];
        char y2 = src[3];

        float r = + 2.032f*float(u);
        float g = - 0.395f*float(u) -0.581f*float(v);
        float b = + 1.140f*float(v);

        dst[0] = float(y1) + b;
        dst[1] = float(y1) + g;
        dst[2] = float(y1) + r;

        dst +=3;

        dst[0] = float(y2) + b;
        dst[1] = float(y2) + g;
        dst[2] = float(y2) + r;

        dst +=3;
    }
}

void Capture::UYVYtoGRAY(const char* src, const char* end, char* dst)
{
    for(; src<end; src+=4)
    {
        *dst++ = src[1];
        *dst++ = src[3];
    }
}

void Capture::YUV422PtoGREY(const char* src, const char* end, char* dst)
{
    memcpy(dst, src, end - src);
}
//-----------------------------------------------------------------------------
