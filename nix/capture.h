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
#include "base_capture.h"
//-----------------------------------------------------------------------------
class Capture: public BaseCapture
{
public:
    Capture(unsigned int covet_w, unsigned int covet_h, const char* dev_name);
    ~Capture();

    const char* capture();
    unsigned int width()const { return captured_width;  }
    unsigned int height()const{ return captured_height; }

    static unsigned int enum_devices(char buffers[][32], unsigned int num)throw()
    {
        return 0;   // Not implemented yet
    }

private:
    void free();

    struct Device
    {
    public:
        Device(const char* name);
        ~Device();

        operator int(){ return fd; }
    private:
        int fd;
    };

    struct MMapBuffer
    {
        MMapBuffer(int fd, size_t len, off_t off);
        ~MMapBuffer();

        MMapBuffer(MMapBuffer&);            // undegined
        MMapBuffer& operator=(MMapBuffer&); // undefined

        void*   start;
        size_t  length;
    };

    void decode_buffer(int index);

    // See http://www.fourcc.org/yuv.php
    //+#define V4L2_PIX_FMT_GREY    v4l2_fourcc('G','R','E','Y') /*  8  Greyscale     */
    //#define V4L2_PIX_FMT_PAL8    v4l2_fourcc('P','A','L','8') /*  8  8-bit palette */
    //#define V4L2_PIX_FMT_YVU410  v4l2_fourcc('Y','V','U','9') /*  9  YVU 4:1:0     */
    //#define V4L2_PIX_FMT_YVU420  v4l2_fourcc('Y','V','1','2') /* 12  YVU 4:2:0     */
    //+#define V4L2_PIX_FMT_YUYV    v4l2_fourcc('Y','U','Y','V') /* 16  YUV 4:2:2     */
    //+#define V4L2_PIX_FMT_UYVY    v4l2_fourcc('U','Y','V','Y') /* 16  YUV 4:2:2     */
    //+#define V4L2_PIX_FMT_YUV422P v4l2_fourcc('4','2','2','P') /* 16  YVU422 planar */
    //#define V4L2_PIX_FMT_YUV411P v4l2_fourcc('4','1','1','P') /* 16  YVU411 planar */
    //#define V4L2_PIX_FMT_Y41P    v4l2_fourcc('Y','4','1','P') /* 12  YUV 4:1:1     */
    //#define V4L2_PIX_FMT_YUV444  v4l2_fourcc('Y','4','4','4') /* 16  xxxxyyyy uuuuvvvv */
    //#define V4L2_PIX_FMT_YUV555  v4l2_fourcc('Y','U','V','O') /* 16  YUV-5-5-5     */
    //#define V4L2_PIX_FMT_YUV565  v4l2_fourcc('Y','U','V','P') /* 16  YUV-5-6-5     */
    //#define V4L2_PIX_FMT_YUV32   v4l2_fourcc('Y','U','V','4') /* 32  YUV-8-8-8-8   */

    static void GREYtoGREY(const char* src, const char* end, char* dst);

    static void YUYVtoRGB (const char* src, const char* end, char* dst);
    static void YUYVtoBGR (const char* src, const char* end, char* dst);
    static void YUYVtoGRAY(const char* src, const char* end, char* dst);

    static void UYVYtoRGB (const char* src, const char* end, char* dst);
    static void UYVYtoBGR (const char* src, const char* end, char* dst);
    static void UYVYtoGRAY(const char* src, const char* end, char* dst);

    static void YUV422PtoGREY(const char* src, const char* end, char* dst);


    unsigned int select_format();

    typedef void (*Decoder)(const char* src, const char* end, char* dst);
    struct Converter
    {
        unsigned int fourcc;
        Decoder decoder[num];
    };

    const static Converter supported_formats[4];

    unsigned int captured_width;
    unsigned int captured_height;

    MMapBuffer**    buffers;
    unsigned int    num_buffers;
    const Decoder*  decoders;
    Device          device;
	
	// thread`s stuff
	static void* capture_frame_thead(void* capture);
	pthread_t worker_thread;
};
//-----------------------------------------------------------------------------
#endif//CAPTURE_H
//-----------------------------------------------------------------------------
