#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <vector>
#include <algorithm>
#include <sys/time.h>
#include "SDL.h"
#define ENABLE_IOSTREAM
#include "transform.h"
#include "canvas.h"

typedef enum { TRIANGLES, TRIANGLES_INDEXED, LINE_STRIP, LINE_LOOP, POINTS } prim_t;

class Renderer {
    Canvas &m_canvas;
    std::vector<vec4f> m_vertices;
    std::vector<vec2f> m_texcoords;
    std::vector<vec3i> m_indeces;
    const Pixman *m_texture;
    Matrix4f m_viewport;
    Matrix4f m_model;
    Matrix4f m_trans;
    bool m_wire;

    void drawPoints()
    {
        for (unsigned i = 0; i < m_vertices.size(); i++) {
            vec4f dot = m_trans * m_vertices[i];
            int z = dot.z();
            dot /= dot.w();
            m_canvas.point(dot.x(), dot.y(), z);
        }
    }

    void drawLines(bool loop = true)
    {
        Vertex v0, v1, v2;

        for (unsigned i = 0; i < m_vertices.size(); i++) {
            vec4f t = m_trans * m_vertices[i];
            vec3f pos = t/t.w();
            pos[2] = t.z();

            if (i == 0) {
                v0 = v1 = pos;
                continue;
            }

            v2 = pos;
            m_canvas.line(v1, v2);
            v1 = v2;

            if (loop && (i+1 == m_vertices.size()))
                m_canvas.line(v1, v0);
        }
    }

    void setVertex(Vertex &vt, size_t n)
    {
        bool texmap = m_texture
            && !m_wire
            && m_vertices.size() <= m_texcoords.size();

        vec4f pos = m_trans * m_vertices[n];
        float z = pos.z();
        pos /= pos.w();

        vt[0] = roundf(pos.x());
        vt[1] = roundf(pos.y());
        vt[2] = z;
        if (texmap) {
            vt[3] = (m_texture->width()-1)*m_texcoords[n].x(); // u
            vt[4] = (m_texture->height()-1)*m_texcoords[n].y(); // v
        }
    }

    void drawTriangle(const Vertex vt[3])
    {
        if (m_wire) {
            m_canvas.line(vt[0], vt[1]);
            m_canvas.line(vt[1], vt[2]);
            m_canvas.line(vt[2], vt[0]);
        } else
            m_canvas.triangle(vt);
    }

    void drawTriangles()
    {
        size_t n = m_vertices.size();
        assert(n % 3 == 0);

        for (size_t i = 0; i < n; i += 3) {
            Vertex vt[3];
            memset(&vt, 0, sizeof(vt));
            for (int j = 0; j < 3; j++)
                setVertex(vt[j], i+j);
            drawTriangle(vt);
        }
    }

    void drawTrianglesIndexed()
    {
        size_t n = m_indeces.size();

        for (size_t i = 0; i < n; i++) {
            Vertex vt[3];
            memset(&vt, 0, sizeof(vt));
            for (int j = 0; j < 3; j++)
                setVertex(vt[j], m_indeces[i][j]);
            drawTriangle(vt);
        }
    }

public:
    Renderer(Canvas &canvas)
        : m_canvas(canvas)
        , m_texture(NULL)
        , m_wire(false)
    {
        float sx = m_canvas.width()/2;
        float sy = m_canvas.height()/2;

        m_model.loadIdentity();
        m_viewport = scale(sx, -sy, 1.0f) * translate(1.0f, -1.0f, 0.0f);
    }

    void transform(const Matrix4f &m)
    {
        m_model = m * m_model;
    }

    void vertexPointer(const float pp[][3], int count)
    {
        m_vertices.clear();
        for (int i = 0; i < count; i++)
            m_vertices.push_back(vec4(pp[i][0], pp[i][1], pp[i][2], 1.f));
    }

    void texcoordPointer(const float tt[][2], int count)
    {
        m_texcoords.clear();
        for (int i = 0; i < count; i++)
            m_texcoords.push_back(vec2(tt[i][0], tt[i][1]));
    }

    void indexPointer(const int ii[][3], int count)
    {
        m_indeces.clear();
        for (int i = 0; i < count; i++)
            m_indeces.push_back(vec3(ii[i][0], ii[i][1], ii[i][2]));
    }

    void texture(const Pixman *texture)
    {
        m_texture = texture;
        m_canvas.texture(texture);
    }

    void wire(bool enable)
    {
        m_wire = enable;
    }

    void render(prim_t mode)
    {
        Matrix4f proj;
        proj.loadIdentity();
        proj[2][3] = 1;
        proj[3][3] = 0;
        m_trans = m_viewport * proj * translate(0.f, 0.f, 1.f) * m_model;

        switch (mode) {
        case TRIANGLES:
            drawTriangles();
            break;
        case TRIANGLES_INDEXED:
            drawTrianglesIndexed();
            break;
        case LINE_LOOP:
            drawLines();
            break;
        case LINE_STRIP:
            drawLines(false);
            break;
        case POINTS:
            drawPoints();
            break;
        }
    }
};

Pixman test_texture(const PixelFormat &format)
{
    int size = 10;
    Pixman tex(size, size, format);

    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++) {
            uint32_t color;
            if ((y+x) & 1)
                color = tex.mapRGB(0x00, 0x00, 0x00);
            else
                color = tex.mapRGB(0xFF, 0xFF, 0xFF);
            tex.set(x, y, color);
        }

    return tex;
}

Pixman sdlPixman(SDL_Surface *sdlSurface)
{
    SDL_PixelFormat *sdlFormat = sdlSurface->format;
    PixelFormat pf;
    pf.bpp = sdlFormat->BytesPerPixel;
    pf.mR = sdlFormat->Rmask;
    pf.mG = sdlFormat->Gmask;
    pf.mB = sdlFormat->Bmask;
    pf.mA = sdlFormat->Amask;
    pf.sR = sdlFormat->Rshift;
    pf.sG = sdlFormat->Gshift;
    pf.sB = sdlFormat->Bshift;
    pf.sA = sdlFormat->Ashift;

    return Pixman(sdlSurface->w, sdlSurface->h,
                  pf, (uint8_t*)sdlSurface->pixels);
}


void testTriangles(Renderer &r)
{
    float pp[][3] = {
        {-0.5, -0.5, 0},
        {-0.5, 0.5, 0},
        {0.5, -0.5, 0},

        {-0.5, 0.5, 0},
        {0.5, -0.5, 0},
        {0.5, 0.5, 0},
    };

    float tt[][2] = {
        {0.0, 0.0},
        {0.0, 1.0},
        {1.0, 0.0},

        {0.0, 1.0},
        {1.0, 0.0},
        {1.0, 1.0},
    };

    r.vertexPointer(pp, 6);
    r.texcoordPointer(tt, 6);
    r.render(TRIANGLES);
}


void testBunny(Renderer &r)
{
#include "bunny.h"
    r.vertexPointer(vertices, sizeof(vertices)/sizeof(vertices[0]));
    r.indexPointer(indeces, sizeof(indeces)/sizeof(indeces[0]));
    float s = 7.0f;
    r.transform(translate(0.f, -0.6f, 0.0f) * scale(s, s, s));
    r.render(TRIANGLES_INDEXED);
}

void testCube(Renderer &r)
{
#include "cube.h"
    r.vertexPointer(vertices, sizeof(vertices)/sizeof(vertices[0]));
    r.indexPointer(indeces, sizeof(indeces)/sizeof(indeces[0]));
    //r.indexPointer(indeces, 1);
    float s = 0.3f;
    r.transform(rotate(-1.1f, 1.f, 1.f, 0.f));
    r.transform(scale(s, s, s));
    r.render(TRIANGLES_INDEXED);
}


int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface *screen = SDL_SetVideoMode(640, 480, 24, SDL_SWSURFACE|SDL_DOUBLEBUF);
    assert(screen != NULL);
    Pixman pscreen = sdlPixman(screen);
    Pixman texture = test_texture(pscreen.format());

    Canvas canvas(pscreen);
    Renderer r(canvas);
    //r.texture(&texture);
    r.wire(true);

    //testCube(r);
    testBunny(r);

    SDL_Flip(screen);

    bool run = true;
    while (run) {
        SDL_Event event;
        SDL_WaitEvent(&event);
        switch (event.type) {
        case SDL_QUIT:
            run = false;
            break;
        default:
            continue;
        }
    }

    SDL_Quit();
	return 0;
}
