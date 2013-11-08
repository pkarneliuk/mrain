//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   matrix.h
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef MATRIX_H
#define MATRIX_H
//-----------------------------------------------------------------------------
#include <cassert>
#include <cmath>
#include <cstring>

#include "blas.h"
#include "gl_renderer.h"
#include "gpu_program.h"
#include "matrix_img.h"
#include "texture_atlas.h"
#include "video_buffer.h"
#include "buffer_object.h"
#include "vertex_data.h"
#include "vao.h"
//-----------------------------------------------------------------------------
class Matrix    // Simply Matrix effect
{
    typedef VertexData::Layout<VertexData::D4UB, VertexData::V3F, VertexData::C4F> D4UB_V3F_C4F;

protected:

    class Twister
    {
    public:
        Twister(float r=3.0f, float h=15.0f, float p=25.0f, float q=6.0f, float rotates=20.0f):radius(r), height(h), a(p), b(q), c(rotates){}

        vector operator()(float t) // 0.0f <= t <= 1.0f
        {
            float p = vertical_form(t) * radius;
            float s = sigmoid(t, a);
            float m = sigmoid(t, b);		
            float r = phi(m, c);
            float x = p * sinf(r);
            float y = s * height;
            float z = p * cosf(r);

            vector v(x, y, z);
            return v;
        }

    private:

        float vertical_form   (float x)const    { return x*x*21.f - x*19.f + 4.5f; }
        float phi    (float x, float n)const    { return x*n*2*3.141926585f; }
        float sigmoid(float x, float a)const    { return 1.0f /(1.0f + exp(-a * (x-0.5f))); }

        float radius;
        float height;
        float a;
        float b;
        float c;
    };

    class Animation
    {
    public:

        Animation( unsigned int n, float size, const vector& a, const vector& b, float h1, float h2, float r, float p, float q, float rotates)
        :nframes(n), spline(key_points, 7), twister(r, h2, p, q, rotates)
        {
            key_points[0] = a;
            key_points[1] = a;

            key_points[2] = a; key_points[2].y += h1;//vector(xa, ya+h1, za);
            key_points[3] = a; key_points[3].y += h1;//vector(xa, ya+h1, za);
         // key_points[3] = vector(xb, yb, zb)+twister(0.0f);
            key_points[4] = b+twister(0.00f);
            key_points[5] = b+twister(0.02f);
            key_points[6] = b+twister(0.02f);
            key_points[7] = b;

        //  key_points[7] = vector(xa, ya, za);
            key_points[8] = a;

            vcache = new VertexData::V3F[nframes * 4];

            for(unsigned int i=0; i<nframes-1; i++)
            {
                vector v0 = get_point(i);
                vector v1 = v0;
                v1.x += size;
                vector v2 = get_point(i+1);
                vector v3 = v2;
                v3.x += size;

                unsigned int index = i<<2; // i*4

                vcache[index].x = v0.x;
                vcache[index].y = v0.y;
                vcache[index].z = v0.z;

                vcache[index+1].x = v1.x;
                vcache[index+1].y = v1.y;
                vcache[index+1].z = v1.z;

                vcache[index+2].x = v2.x;
                vcache[index+2].y = v2.y;
                vcache[index+2].z = v2.z;

                vcache[index+3].x = v3.x;
                vcache[index+3].y = v3.y;
                vcache[index+3].z = v3.z;
            }
        }
        Animation& operator=(Animation&); //undefined

        ~Animation()
        {
            delete[] vcache;
        }

        inline void vertexcpy(VertexData::V3F* array, unsigned int num, unsigned int begin_frame)
        {
            assert(sizeof(vector) == sizeof(VertexData::V3F));
            memcpy(array, vcache+(begin_frame*4), sizeof(vector)*num );
        }

        const unsigned int nframes;

    private:
        vector get_point(unsigned int frame)
        {
            frame %= nframes;
            float t = float(frame)/nframes;

            const float s = 0.2f;
            if(t< s)
            {
                return spline(t/s);
            }
            else return key_points[7]+twister(0.018f +(t-s)/(1.0f-s) );
        }

        vector key_points[9];
        HSpline spline;
        Twister twister;
        VertexData::V3F* vcache;
    };


    class Strip
    {
    public:
        Strip(unsigned int n, VertexData::D4UB* g, VertexData::V3F* v, VertexData::C4F* c,
        GLfloat x, GLfloat y, GLfloat z, const vector& ac, float h1, float h2, float r, float p, float q, float rotates);
        Animation& operator=(Animation&); //undefined
        ~Strip();

        void draw(GLint* first, GLsizei* count);
        void tick(unsigned long usec);

        VertexData::D4UB*   glyphs;
        VertexData::V3F*    vertexies;
        VertexData::C4F*    colors;
        GLfloat size;

        Animation animation;

        Counter wave_waiter;    // a wave`s lifecycle
        Counter aframe_waiter;  // an animation frame`s lifecycle


        unsigned int n_glyphs;  // total glyphs
        unsigned int end_glyph; // last glyph
        unsigned int wavehead;  // head of wave

        int adelay;             // animation delay bad idea, it will be refactored :(
        unsigned int aframe;    // animation frame
        bool arunning;          // animation running

    private:
        void wave_tick(unsigned long usec);
    };


public:
    Matrix(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture);
    virtual ~Matrix();

    void draw(const Transform& transform);
    void tick(unsigned long usec);

    virtual void build_program();
    virtual void pre_draw(const Transform& transform);
    virtual void post_draw();

protected:

    // array of spawners
    typedef void (Matrix::*spawn)();
    static const Matrix::spawn spawners[];

    void spawn_a();
    void spawn_b();
    void spawn_c();
    void spawn_d();

    TextureAtlas::Texture* letter;

    VertexData::D4UB* glyphs;
    VertexData::V3F*  vertexies;
    VertexData::C4F*  colors;

    GLint*      firsts;
    GLsizei*    counts;

    GLfloat*          data;
    VBO<D4UB_V3F_C4F> vbo;
    VAO vao;
    GPU_Program program;
    matrix model;

    unsigned int nstrips;   // Number of strips
    unsigned int nglyphs;   // Number of glyphs
    Strip** strips;
    Random grid_random;

    Waiter animation_period;
};


class MatrixVideo:public Matrix // Matrix and Video effect
{
public:
    MatrixVideo(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture, const VideoBuffer* buffer, int width, int height, bool vertical_flip, bool horizontal_flip);
    ~MatrixVideo();

    virtual void build_program();
    virtual void pre_draw(const Transform& transform);
    virtual void post_draw();

protected:
    const VideoBuffer* video;   // Texture instance
    bool vflip;
    bool hflip;
};
//-----------------------------------------------------------------------------
#endif//MATRIX_H
//-----------------------------------------------------------------------------
