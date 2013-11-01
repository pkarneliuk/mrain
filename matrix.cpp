//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   matrix.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "matrix.h"
#include "vertex_data.h"
#include "vao.h"
//-----------------------------------------------------------------------------
const Matrix::spawn Matrix::spawners[]={&Matrix::spawn_a,
                                        &Matrix::spawn_b,
                                        &Matrix::spawn_c,
                                        &Matrix::spawn_d, };
//-----------------------------------------------------------------------------
Matrix::Matrix(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture): letter(texture),
    glyphs(NULL),
    vertexies(NULL),
    colors(NULL),
    firsts(NULL), counts(NULL),
    nstrips(ns), nglyphs(ng),
    strips(NULL), grid_random(324467), animation_period(100000000 + grandom(50000000U))
{
    unsigned int strip_pack = nglyphs * 4;

    const unsigned int num_vertices = nstrips * strip_pack;

    static_assert(sizeof(D4UB_V3F_C4F) == 32, "wrong sizeof");

    const size_t vsize = sizeof(D4UB_V3F_C4F);
    data = new GLfloat[vsize/sizeof(GLfloat) * num_vertices];

    glyphs    = reinterpret_cast<VertexData::D4UB*>(&data[0]);
    vertexies = reinterpret_cast<VertexData::V3F*>(&data[  sizeof(VertexData::D4UB)/sizeof(GLfloat) * num_vertices]);
    colors    = reinterpret_cast<VertexData::C4F*>(&data[ (sizeof(VertexData::D4UB) + sizeof(VertexData::V3F))/sizeof(GLfloat) * num_vertices]);

    firsts = new GLint[nstrips];
    counts = new GLsizei[nstrips];
    strips = new Strip*[nstrips];

    for(unsigned int i=0; i<nstrips; i++)
    {
        strips[i] = new Strip( strip_pack,
                        &glyphs[i*strip_pack],
                        &vertexies[i*strip_pack],
                        &colors[i*strip_pack],
                        float(i)/2.0f, -grid_random(5.0f), 0,
                        -48.0f, 35.0f, grid_random(5.0f)+2.0f,
                        grid_random(10.0f)+15.0f, grid_random(1.0f)+5.0f, grid_random(10.0f)+10.0f);
    }

    vbo.bind();
        vbo.create(num_vertices, data, GL_STREAM_DRAW);
        vao.bind();
            vao.bind(vbo, num_vertices, 0);
        vao.unbind();
    vbo.unbind();

    model.identity();
    model.translate(vector(-32.0,26.0,-10.0f));
}

Matrix::~Matrix()
{
    for(unsigned int i=0; i<nstrips; i++)
    {
        delete strips[i];
    }
    delete[] strips;
    delete[] counts;
    delete[] firsts;

    delete[] data;
}

void Matrix::build_program()
{
    Shader vshader(Shader::Vertex);
    const GLchar* vertex_shader = 
    "#version 130\n"
    "uniform mat4 transform;"
    "in  uvec4 data;"
    "in  vec3 position;"
    "in  vec4 color;"
    "out vec4 ex_color;"
    "out vec2 texcoord;"
    "void main(void)"
    "{"
    "    gl_Position = transform * vec4(position, 1.0);"
    "    ex_color = color;"
    // optimization of:
    //"    texcoord.t = ((gl_VertexID % 4) < 2) ? 1.0 : 0.0;"
    "    texcoord.t = (~gl_VertexID & 0x3) >> 1;"
    "    texcoord.s = float(data.s +  uint(gl_VertexID & 0x1) )/32.0;"
    "}";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    Shader fshader(Shader::Fragment);
    const GLchar* fragment_shader = 
    "#version 130\n"
    "uniform sampler2D glyphs;"
    "in vec4 ex_color;"
    "in  vec2 texcoord;"
    "out vec4 fragment;"
    "void main(void)"
    "{"
    "    vec4 t = texture(glyphs, texcoord);"
    "    if(t.r ==0) discard;"
    "    vec4 c = ex_color;"
//    "    c.a =  1 - ex_color.a;"
//    "    ex_color.a = 1 - ex_color.a;"
    "    fragment = c*t.r;"
    "}";

    fshader.set_source(fragment_shader);
    fshader.compile();
    fshader.log();

    program.attach(vshader);
    program.attach(fshader);
    program.bind(0, "data");
    program.bind(1, "position");
    program.bind(2, "color");

    program.link();
    program.validate();
    program.log();
}

void Matrix::pre_draw(const Transform& transform)
{
    // No video

//    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);

    letter->bind();

    program.use();
    program.set_sampler("glyphs", 0);
    vao.bind();

    transform.bind_transform(program, model);
}

void Matrix::draw(const Transform& transform)
{
    pre_draw(transform);

    for(unsigned int i=0; i<nstrips; i++)
    {
        strips[i]->draw(&firsts[i], &counts[i]);
        firsts[i] += (i * nglyphs<<2);
    }

//    glMultiDrawArrays(GL_TRIANGLE_STRIP, firsts, counts, nstrips);
    for(unsigned int i=0; i<nstrips; i++)
        glDrawArrays(GL_TRIANGLE_STRIP, firsts[i], counts[i]);

    post_draw();
}

void Matrix::post_draw()
{
    vao.unbind();
    GPU_Program::use_default();
    glDisable(GL_BLEND);
//    glDepthMask(GL_TRUE);
}

void Matrix::tick(unsigned long usec)
{
    if( animation_period.ready() )
    {
        // select random spawner
        unsigned int index = grandom((unsigned int)(sizeof(Matrix::spawners)/sizeof(Matrix::spawn)));
        (this->*Matrix::spawners[index])();
    }

    for(unsigned int i=0; i<nstrips; i++)
    {
        strips[i]->tick(usec);
    }

    vbo.bind();
        vbo.set_data(0, nstrips * nglyphs * 4 * sizeof(D4UB_V3F_C4F), data);
    vbo.unbind();
}

void Matrix::spawn_a()
{
    for(unsigned int i=0; i<nstrips; i++)
    {
        if( !strips[i]->arunning )
        {
            strips[i]->arunning = true;
#ifdef UNIX
            unsigned int delay = abs(i-nstrips/2) * 120000;
#else
            unsigned int delay = abs(int(i-nstrips/2)) * 120000;
#endif
            strips[i]->adelay = delay + grandom(30000U);
        }
    }
}

void Matrix::spawn_b()
{
    for(unsigned int i=0; i<nstrips; i++)
    {
        if( !strips[i]->arunning )
        {
            strips[i]->arunning = true;
            unsigned int delay = i * 80000;
            strips[i]->adelay = delay + grandom(30000U);
        }
    }
}

void Matrix::spawn_c()
{
    for(unsigned int i=0; i<nstrips; i++)
    {
        if( !strips[i]->arunning )
        {
            strips[i]->arunning = true;
            unsigned int delay = (nstrips - i) * 80000;
            strips[i]->adelay = delay + grandom(30000U);
        }
    }
}

void Matrix::spawn_d()
{
    for(unsigned int i=0; i<nstrips; i++)
    {
        if( !strips[i]->arunning )
        {
            strips[i]->arunning = true;
            strips[i]->adelay = grandom(2500000U);
        }
    }
}

Matrix::Strip::Strip(unsigned int n, VertexData::D4UB* g, VertexData::V3F* v, VertexData::C4F* c,
GLfloat x, GLfloat y, GLfloat z, float h1, float h2, float r, float p, float q, float rotates)
    : glyphs(g)
    , vertexies(v)
    , colors(c)
    , size(0.4f)
    , animation(2048, size, vector(x, y, z), vector(32.0f, -35.0f, -35.0f), h1, h2, r, p, q, rotates)
    , wave_waiter(100000 + grandom(40000U))
    , aframe_waiter(10000)
    , n_glyphs(n/4)
    , end_glyph(0)
    , wavehead(0)
    , adelay(0)
    , aframe(0)
    , arunning(false)
{
    GLfloat yi = y;
    for(unsigned int i=0; i<n; i+= 4)
    {
        GLfloat a = cosf(float(( int(wavehead - (i/4)) %80))*(3.1415926f/180.0f));

        GLubyte g = rand()%32;

        glyphs[i].i = g;
        colors[i].r = 0.0f;
        colors[i].g = 1.0f;
        colors[i].b = 0.0f;
        colors[i].a = a;
        vertexies[i].x = x;
        vertexies[i].y = yi;
        vertexies[i].z = z;

        glyphs[i+1].i = g;
        colors[i+1].r = 0.0f;
        colors[i+1].g = 1.0f;
        colors[i+1].b = 0.0f;
        colors[i+1].a = a;
        vertexies[i+1].x = x+size;
        vertexies[i+1].y = yi;
        vertexies[i+1].z = z;

        glyphs[i+2].i = g;
        colors[i+2].r = 0.0f;
        colors[i+2].g = 1.0f;
        colors[i+2].b = 0.0f;
        colors[i+2].a = a;
        vertexies[i+2].x = x;
        vertexies[i+2].y = yi-size;
        vertexies[i+2].z = z;

        glyphs[i+3].i = g;
        colors[i+3].r = 0.0f;
        colors[i+3].g = 1.0f;
        colors[i+3].b = 0.0f;
        colors[i+3].a = a;
        vertexies[i+3].x = x+size;
        vertexies[i+3].y = yi-size;
        vertexies[i+3].z = z;

        yi -= size;
    }
}

Matrix::Strip::~Strip()
{
}

void Matrix::Strip::draw(GLint* first, GLsizei* count)
{
    *first = 0;
    *count = end_glyph*4;
}

void Matrix::Strip::tick(unsigned long usec)
{
    wave_tick(usec);

    if(arunning)
    {
        if(adelay > 0)
        {
            adelay -= usec;
        }
        else
        {
            if(unsigned int iframe = aframe_waiter.test(usec))
            {
                aframe += iframe;
                if( aframe >= animation.nframes )   // animation ends
                {
                    aframe = 0;
                    arunning = false;   // stop animation
                }

            //  end_glyph = n_glyphs;

                unsigned int w = animation.nframes - aframe;
                unsigned int k = n_glyphs;

                if(k > w) end_glyph = 0;
                else animation.vertexcpy(vertexies, k*4, aframe);
            }
        }
    }
}

void Matrix::Strip::wave_tick(unsigned long usec)
{
    if( unsigned int inc = wave_waiter.test(usec) )
    {
        if(end_glyph < n_glyphs)
        {
            end_glyph += inc;
            if(end_glyph > n_glyphs) end_glyph = n_glyphs;
        }

        const unsigned int per = 64;        // period of waves
        wavehead = (wavehead + inc) % per;  // clump to [0:per]

        assert(end_glyph <= n_glyphs);

        for(unsigned int i=0; i<end_glyph; i++)
        {
            unsigned int index = i<<2;  // index = i*4;
            unsigned int iper = (wavehead - i) % per;
            if(iper != 0)
            {
                float s = (float(iper) / float(per)) * 50.0f;
                GLfloat a = sinf(s*(3.1415926f/180.0f));

                colors[index+0].a = a;
                colors[index+1].a = a;
                colors[index+2].a = a;
                colors[index+3].a = a;
            }
            else
            {
                // change glyph image
                GLubyte g = rand()%32;

                glyphs[index+0].i = g;
                glyphs[index+1].i = g;//(g+1)%32;
                glyphs[index+2].i = g;
                glyphs[index+3].i = g;//(g+1)%32;

                colors[index+0].a = 0.0f;
                colors[index+1].a = 0.0f;
                colors[index+2].a = 0.0f;
                colors[index+3].a = 0.0f;
            }
        }
    }
}

MatrixVideo::MatrixVideo(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture, const VideoBuffer* buffer, int /*width*/, int /*height*/, bool vflip, bool hflip)
    :Matrix(ns, ng, texture), video(buffer)
{
    /*
    video_st = new VertexData::T2F[ns * ng * 4];    // Vertex array for video texture unit

    float video_res[4];
    video_res[0] = 64.0f;
    video_res[1] = 48.0f;
    video_res[2] = video_res[0]/buffer->s;
    video_res[3] = video_res[1]/buffer->t;

    const unsigned int n = nstrips * nglyphs * 4;

    if(vflip)
        for(unsigned int i=0; i<n; i++)
        {
            video_st[i].s = (video_res[0] - vertexies[i].x)/video_res[2];
        }
    else
        for(unsigned int i=0; i<n; i++)
        {
            video_st[i].s = vertexies[i].x/video_res[2];
        }

    if(hflip)
        for(unsigned int i=0; i<n; i++)
        {
            video_st[i].t = -(vertexies[i].y/video_res[3]);
        }
    else
        for(unsigned int i=0; i<n; i++)
        {
            video_st[i].t = buffer->t+(vertexies[i].y/video_res[3]);
        }
        */
}

MatrixVideo::~MatrixVideo()
{
}

void MatrixVideo::build_program()
{
    Shader vshader(Shader::Vertex);
    const GLchar* vertex_shader = 
    "#version 130\n"
    "uniform mat4 transform;"
    "in  uvec4 data;"
    "in  vec3 position;"
    "in  vec4 color;"
    "out vec4 ex_color;"
    "out vec2 texcoord;"
    "void main(void)"
    "{"
    "    gl_Position = transform * vec4(position, 1.0);"
    "    ex_color = color;"
    // optimization of:
    //"    texcoord.t = ((gl_VertexID % 4) < 2) ? 1.0 : 0.0;"
    "    texcoord.t = (~gl_VertexID & 0x3) >> 1;"
    "    texcoord.s = float(data.s +  uint(gl_VertexID & 0x1) )/32.0;"
    "}";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    Shader fshader(Shader::Fragment);
    const GLchar* fragment_shader = 
    "#version 130\n"
    "uniform vec4 viewport;"
    "uniform sampler2D glyphs;"
    "uniform sampler2D video;"
    "in vec4 ex_color;"
    "in  vec2 texcoord;"
    "out vec4 fragment;"
    "void main(void)"
    "{"
    "    vec4 t = texture(glyphs, texcoord);"
    "    if(t.r == 0) discard;"

    "    vec2 video_st = gl_FragCoord.xy/viewport.zw;"
    "    vec4 v = texture(video, video_st);"

    "    float gray = dot(v.rgb, vec3(0.2125, 0.7154, 0.0721));"
    "    fragment = vec4(ex_color.rgb * min(t.r, gray), ex_color.a);"
//    "    fragment = vec4( mix(ex_color.rgb, v.rgb, t.r), ex_color.a);"
//    "    fragment = vec4(v.rgb, ex_color.a);"
    "}";

    fshader.set_source(fragment_shader);
    fshader.compile();
    fshader.log();

    program.attach(vshader);
    program.attach(fshader);
    program.bind(0, "data");
    program.bind(1, "position");
    program.bind(2, "color");

    program.link();
    program.validate();
    program.log();
}

void MatrixVideo::pre_draw(const Transform& transform)
{
//  Video & fixed pipeline
    Matrix::pre_draw(transform);

    transform.bind_viewport(program);

    glActiveTexture(GL_TEXTURE1);
    program.set_sampler("video", 1);

    video->frame()->bind();
}

void MatrixVideo::post_draw()
{
    Matrix::post_draw();
}
//-----------------------------------------------------------------------------
