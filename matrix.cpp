//-----------------------------------------------------------------------------
// "Matrix Rain" - screensaver for X Server Systems
// file name:   matrix.cpp
// copyright:   (C) 2008, 2009 by Pavel Karneliuk
// license:     GNU General Public License v2
// e-mail:      pavel_karneliuk@users.sourceforge.net
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "matrix.h"
//-----------------------------------------------------------------------------
const Matrix::spawn Matrix::spawners[]={&Matrix::spawn_a,
                                        &Matrix::spawn_b,
                                        &Matrix::spawn_c,
                                        &Matrix::spawn_d, };
//-----------------------------------------------------------------------------
Matrix::Matrix(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture): letter(texture),
    glyph_st(NULL),
    vertexies(NULL),
    colors(NULL),
    firsts(NULL), counts(NULL),
    nstrips(ns), nglyphs(ng),
    strips(NULL), grid_random(324467), animation_period(180000000 + grandom(90000000U))
{
    unsigned int strip_pack = nglyphs * 4;

    glyph_st    = new GLRenderer::T2F[nstrips * strip_pack];
    vertexies   = new GLRenderer::V3F[nstrips * strip_pack];
    colors      = new GLRenderer::C4F[nstrips * strip_pack];

    firsts = new GLint[nstrips];
    counts = new GLsizei[nstrips];
    strips = new Strip*[nstrips];

    for(unsigned int i=0; i<nstrips; i++)
    {
        strips[i] = new Strip(	strip_pack,
                        &glyph_st[i*strip_pack],
                        &vertexies[i*strip_pack],
                        &colors[i*strip_pack],
                        float(i)/2.0f, -grid_random(3.0f), 0,
                        -48.0f, 35.0f, grid_random(5.0f)+2.0f,
                        grid_random(10.0f)+15.0f, grid_random(1.0f)+5.0f, grid_random(10.0f)+10.0f);
    }
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

    delete[] colors;
    delete[] vertexies;
    delete[] glyph_st;
}

void Matrix::draw()
{
    pre_draw();
    for(unsigned int i=0; i<nstrips; i++)
    {
        strips[i]->draw(&firsts[i], &counts[i]);
        firsts[i] += (i * nglyphs<<2);
    }

    glMultiDrawArrays(GL_TRIANGLE_STRIP, firsts, counts, nstrips);
    post_draw();
}

void Matrix::tick(unsigned long usec)
{
    if( animation_period.ready() )
    {
        // select random spawner
        unsigned int index = grandom(sizeof(Matrix::spawners)/sizeof(Matrix::spawn));
        (this->*Matrix::spawners[index])();
    }

    for(unsigned int i=0; i<nstrips; i++)
    {
        strips[i]->tick(usec);
    }
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

void Matrix::pre_draw()
{
    // No video & fixed pipeline
    glPushAttrib(GL_TEXTURE_BIT|GL_ENABLE_BIT);     // Save server state
    glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT); // Save client state

//  glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    letter->bind();

    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);
    glClientActiveTexture(GL_TEXTURE0);

    glTexCoordPointer(2, GL_FLOAT, sizeof(GLRenderer::T2F), &glyph_st[0].st);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glColorPointer   (4, GL_FLOAT, sizeof(GLRenderer::C4F), &colors[0].rgba);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer  (3, GL_FLOAT, sizeof(GLRenderer::V3F), &vertexies[0].xyz);
    glEnableClientState(GL_VERTEX_ARRAY);
}

void Matrix::post_draw()
{
//  glDepthMask(GL_TRUE);
    glPopClientAttrib();// Load client state
    glPopAttrib();      // Load server state
}

Matrix::Strip::Strip(unsigned int n, GLRenderer::T2F* g, GLRenderer::V3F* v, GLRenderer::C4F* c,
GLfloat x, GLfloat y, GLfloat z, float h1, float h2, float r, float p, float q, float rotates): 
glyph_st(g), vertexies(v), colors(c), size(0.4f), animation(2048, size,
 vector(x, y, z), vector(32.0f, -35.0f, -35.0f), h1, h2, r, p, q, rotates), wave_waiter(100000 + grandom(40000U)), aframe_waiter(10000), n_glyphs(n/4), end_glyph(0), wavehead(0), adelay(0), aframe(0), arunning(false)
{
    GLfloat yi = y;
    for(unsigned int i=0; i<n; i+= 4)
    {
        GLfloat a = 1.0f - cosf(float((int(wavehead - (i/4))%80))*(3.1415926f/180.0f));

        GLfloat s = float(rand()%32)/32.0f;

        glyph_st[i].s = s;
        glyph_st[i].t = 1.0f;
        colors[i].r = 0.0f;
        colors[i].g = 1.0f;
        colors[i].b = 0.0f;
        colors[i].a = a;
        vertexies[i].x = x;
        vertexies[i].y = yi;
        vertexies[i].z = z;

        glyph_st[i+1].s = s+1.0f/32.0f;
        glyph_st[i+1].t = 1.0f;
        colors[i+1].r = 0.0f;
        colors[i+1].g = 1.0f;
        colors[i+1].b = 0.0f;
        colors[i+1].a = a;
        vertexies[i+1].x = x+size;
        vertexies[i+1].y = yi;
        vertexies[i+1].z = z;

        glyph_st[i+2].s = s;
        glyph_st[i+2].t = 0.0f;
        colors[i+2].r = 0.0f;
        colors[i+2].g = 1.0f;
        colors[i+2].b = 0.0f;
        colors[i+2].a = a;
        vertexies[i+2].x = x;
        vertexies[i+2].y = yi-size;
        vertexies[i+2].z = z;

        glyph_st[i+3].s = s+1.0f/32.0f;
        glyph_st[i+3].t = 0.0f;
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
            if(end_glyph > n_glyphs)
                end_glyph = n_glyphs;
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
                GLfloat sa = float(rand()%32)/32.0f;
                GLfloat sb = sa+1.0f/32.0f;

                glyph_st[index].s = sa;
                glyph_st[index].t = 1.0f;
                glyph_st[index+1].s = sb;
                glyph_st[index+1].t = 1.0f;
                glyph_st[index+2].s = sa;
                glyph_st[index+2].t = 0.0f;
                glyph_st[index+3].s = sb;
                glyph_st[index+3].t = 0.0f;

                colors[index+0].a = 0.0f;
                colors[index+1].a = 0.0f;
                colors[index+2].a = 0.0f;
                colors[index+3].a = 0.0f;
            }
        }
    }
}

MatrixVideo::MatrixVideo(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture, const VideoBuffer* buffer, int widht, int height, bool vflip, bool hflip)
    :Matrix(ns, ng, texture), video_st(NULL), video(NULL)
{
    video_st = new GLRenderer::T2F[ns * ng * 4];    // Vertex array for video texture unit

    video = buffer;

    float video_res[4];
    video_res[0] = 64.0f;
    video_res[1] = 48.0f;
    video_res[2] = video_res[0]/buffer->s+0.5f;
    video_res[3] = video_res[1]/buffer->t;

    const unsigned int n = nstrips * nglyphs * 4;
    
    if(vflip)
        for(unsigned int i=0; i<n; i++)
        {
            video_st[i].s = (video_res[0] - vertexies[i].x)/(video_res[2]+0.5f);
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
            video_st[i].t = 1.0f+(vertexies[i].y/video_res[3]);
        }

/*      //      Vertical flip
        video_st[i].s = vertexies[i].x/video_res[2];
        //      Horisontal flip
        video_st[i].t = 1.0f+(vertexies[i].y/video_res[3]);
        // Old UNIX
        video_st[i].s = (video_res[0] - vertexies[i].x)/(video_res[2]+0.5f);
        video_st[i].t = -(vertexies[i].y/video_res[3]);
*/
}

MatrixVideo::~MatrixVideo()
{
    delete[] video_st;
}

void MatrixVideo::pre_draw()
{
//  Video & fixed pipeline
    Matrix::pre_draw();

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    video->bind(GL_MODULATE);

    glClientActiveTexture(GL_TEXTURE1);
    glTexCoordPointer(2, GL_FLOAT, 0, &video_st[0].st);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void MatrixVideo::post_draw()
{
    Matrix::post_draw();
}

MatrixVideoFX::MatrixVideoFX(unsigned int ns, unsigned int ng, TextureAtlas::Texture* texture, const VideoBuffer* buffer, int widht, int height, bool vflip, bool hflip)
:MatrixVideo(ns, ng, texture, buffer, widht, height, vflip, hflip), program(NULL)
{
    Shader vshader(Shader::Vertex);

    const GLchar* vertex_shader = 
    "uniform sampler2D video;"
    "void main(void)"
    "{"
    " vec4 color = texture2D(video, gl_MultiTexCoord1.st);"
    " vec4 vertex = gl_Vertex;"
    " vertex.z += color.r*1.4;"
    " gl_FrontColor = gl_Color;"
    " gl_TexCoord[0] = gl_MultiTexCoord0;"
    " gl_TexCoord[1] = gl_MultiTexCoord1;"
    " gl_Position = gl_ModelViewProjectionMatrix * vertex;"
    "}";

    vshader.set_source(vertex_shader);
    vshader.compile();
    vshader.log();

    program = new GPU_Program;

    program->attach(vshader);
    program->link();
    program->validate();
    program->log();
}

MatrixVideoFX::~MatrixVideoFX()
{
    delete program;
}

void MatrixVideoFX::pre_draw()
{
//  Video & Vertex shader
    MatrixVideo::pre_draw();

    program->use(); // Enable shader
    program->set_sampler("video", 1);
}

void MatrixVideoFX::post_draw()
{
    GPU_Program::use_default(); // Disable shader, use fixed pipeline
    MatrixVideo::pre_draw();
}
//-----------------------------------------------------------------------------
