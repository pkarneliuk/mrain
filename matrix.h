//------------------------------------------------------------------------------
// "Matrix Rain" - Interactive screensaver with webcam integration
// copyright:   (C) 2008, 2009, 2013, 2017 by Pavel Karneliuk
// license:     GNU General Public License v3
// e-mail:      pavel.karneliuk@gmail.com
//------------------------------------------------------------------------------
#pragma once
//------------------------------------------------------------------------------
#include "blas.h"
#include "buffer_object.h"
#include "gl_renderer.h"
#include "gpu_program.h"
#include "texture_atlas.h"
#include "vao.h"
#include "vertex_data.h"
#include "video_buffer.h"
#include <cassert>
#include <cmath>
#include <cstring>
//------------------------------------------------------------------------------
class Matrix : noncopyable// Simply Matrix effect
{
    using D4UB_V3F_C4F =
        VertexData::Layout<VertexData::D4UB, VertexData::V3F, VertexData::C4F>;

    static_assert(sizeof(D4UB_V3F_C4F) == 32, "wrong sizeof");

protected:
    class Twister
    {
    public:
        Twister(float r = 3.0f, float h = 15.0f, float p = 25.0f,
                float q = 6.0f, float rotates = 20.0f)
        : radius(r)
        , height(h)
        , a(p)
        , b(q)
        , c(rotates)
        {
        }

        vector operator()(float t)// 0.0f <= t <= 1.0f
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
        float vertical_form(float x) const
        {
            return x * x * 21.f - x * 19.f + 4.5f;
        }
        float phi(float x, float n) const { return x * n * 2 * 3.141926585f; }
        float sigmoid(float x, float y) const
        {
            return 1.0f / (1.0f + exp(-y * (x - 0.5f)));
        }

        float radius;
        float height;
        float a, b, c;
    };

    class Animation : noncopyable
    {
    public:
        Animation(unsigned int n, float size, const vector& a, const vector& b,
                  float h1, float h2, float r, float p, float q, float rotates)
        : nframes(n)
        , spline(key_points, 7)
        , twister(r, h2, p, q, rotates)
        , vcache(nframes * 4)
        {
            key_points[0] = a;
            key_points[1] = a;

            key_points[2] = a;
            key_points[2].y += h1;// vector(xa, ya+h1, za);
            key_points[3] = a;
            key_points[3].y += h1;// vector(xa, ya+h1, za);
            // key_points[3] = vector(xb, yb, zb)+twister(0.0f);
            key_points[4] = b + twister(0.00f);
            key_points[5] = b + twister(0.02f);
            key_points[6] = b + twister(0.02f);
            key_points[7] = b;

            //  key_points[7] = vector(xa, ya, za);
            key_points[8] = a;

            for(std::size_t i = 0; i < nframes; i++) {
                vector v0 = get_point(i);
                vector v1 = v0;
                v1.x += size;
                vector v2 = get_point(i + 1);
                vector v3 = v2;
                v3.x += size;

                const std::size_t index = i * 4;

                vcache[index + 0].x = v0.x;
                vcache[index + 0].y = v0.y;
                vcache[index + 0].z = v0.z;

                vcache[index + 1].x = v1.x;
                vcache[index + 1].y = v1.y;
                vcache[index + 1].z = v1.z;

                vcache[index + 2].x = v2.x;
                vcache[index + 2].y = v2.y;
                vcache[index + 2].z = v2.z;

                vcache[index + 3].x = v3.x;
                vcache[index + 3].y = v3.y;
                vcache[index + 3].z = v3.z;
            }
        }

        void vertexcpy(VertexData::V3F* array, std::size_t num,
                       std::size_t begin_frame)
        {
            assert(sizeof(vector) == sizeof(VertexData::V3F));
            memcpy(array, vcache.data() + (begin_frame * 4),
                   sizeof(vector) * num);
        }

        const std::size_t nframes;

    private:
        vector get_point(std::size_t frame)
        {
            frame %= nframes;
            float t = float(frame) / nframes;

            const float s = 0.2f;
            if(t < s)
            {
                return spline(t / s);
            }
            else
                return key_points[7] + twister(0.018f + (t - s) / (1.0f - s));
        }

        vector                       key_points[9];
        HSpline                      spline;
        Twister                      twister;
        std::vector<VertexData::V3F> vcache;
    };

    class Strip
    {
    public:
        Strip(std::size_t n, VertexData::D4UB* glyphs,
              VertexData::V3F* vertices, VertexData::C4F* colors, GLfloat x,
              GLfloat y, GLfloat z, const vector& ac, float h1, float h2,
              float r, float p, float q, float rotates);

        void draw(GLint* first, GLsizei* count);
        void tick(VertexData::D4UB* glyphs, VertexData::V3F* vertices,
                  VertexData::C4F* colors, unsigned long usec);

        const GLfloat size;

        Animation animation;

        Counter wave_waiter;  // a wave`s lifecycle
        Counter aframe_waiter;// an animation frame`s lifecycle

        std::size_t n_glyphs; // total glyphs
        std::size_t end_glyph;// last glyph
        std::size_t wavehead; // head of wave
        std::size_t adelay;// animation delay bad idea, it will be refactored :(
        std::size_t aframe;// animation frame
        bool        arunning;// animation running

    private:
        void wave_tick(VertexData::D4UB* glyphs, VertexData::V3F* vertices,
                       VertexData::C4F* colors, unsigned long usec);
    };

public:
    Matrix(std::size_t ns, std::size_t ng, TextureAtlas::Texture* texture);
    virtual ~Matrix() = default;

    void draw(const Transform& transform);
    void tick(unsigned long usec);

    virtual void build_program();
    virtual void pre_draw(const Transform& transform);
    virtual void post_draw();

protected:
    // array of spawners
    using spawn = void (Matrix::*)();
    static const Matrix::spawn spawners[];

    void spawn_a();
    void spawn_b();
    void spawn_c();
    void spawn_d();

    TextureAtlas::Texture* letter;

    std::vector<GLint>   firsts;
    std::vector<GLsizei> counts;

    VBO<D4UB_V3F_C4F> vbo;
    VAO               vao;
    GPU_Program       program;
    matrix            model;

    std::size_t                         nstrips;// Number of strips
    std::size_t                         nglyphs;// Number of glyphs
    std::vector<std::unique_ptr<Strip>> strips;
    Random                              grid_random;

    Waiter animation_period;
};

class MatrixVideo : public Matrix// Matrix and Video effect
{
public:
    MatrixVideo(unsigned int ns, unsigned int ng,
                TextureAtlas::Texture* texture, const VideoBuffer* buffer,
                bool vertical_flip, bool horizontal_flip);

    void build_program() override;
    void pre_draw(const Transform& transform) override;
    void post_draw() override;

protected:
    const VideoBuffer* video;// Texture instance
    bool               vflip;
    bool               hflip;
};
//------------------------------------------------------------------------------
