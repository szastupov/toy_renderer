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

template <size_t N, typename T>
struct VertexArray {
    const T *data;
    const size_t size;
    const T* operator[](int i) const
    {
        return &data[i*N];
    }
};

struct VertexBuffer {
    VertexArray<3, float> vertices;
    VertexArray<3, int> indeces;
    VertexArray<3, float> normals;
    VertexArray<2, float> texcoords;
};

static vec4f vec4fp(const float *src)
{
    vec4f res;
    for (int i = 0; i < 3; i++)
        res[i] = src[i];
    res[3] = 1;
    return res;
}

class Renderer {
    Canvas &m_canvas;
    const VertexBuffer *m_vbuffer;
    const Pixman *m_texture;
    Matrix4f m_viewport;
    Matrix4f m_model;
    Matrix4f m_trans;
    bool m_wire;

    void drawPoints()
    {
        for (size_t i = 0; i < m_vbuffer->vertices.size; i++) {
            vec4f dot = m_trans * vec4fp(m_vbuffer->vertices[i]);
            int z = dot.z();
            dot /= dot.w();
            m_canvas.point(dot.x(), dot.y(), z);
        }
    }

    void drawLines(bool loop = true)
    {
        Vertex v0, v1, v2;
        size_t n = m_vbuffer->vertices.size;

        for (unsigned i = 0; i < n; i++) {
            vec4f t = m_trans * vec4fp(m_vbuffer->vertices[i]);
            vec3f pos = t/t.w();
            pos[2] = t.z();

            if (i == 0) {
                v0 = v1 = pos;
                continue;
            }

            v2 = pos;
            m_canvas.line(v1, v2);
            v1 = v2;

            if (loop && (i+1 == n))
                m_canvas.line(v1, v0);
        }
    }

    void setVertex(Vertex &vt, size_t n)
    {
        bool texmap = m_texture
            && !m_wire
            && m_vbuffer->vertices.size <= m_vbuffer->texcoords.size;

        vec4f pos = m_trans * vec4fp(m_vbuffer->vertices[n]);
        float z = pos.z();
        pos /= pos.w();

        vt[0] = roundf(pos.x());
        vt[1] = roundf(pos.y());
        vt[2] = z;
        if (texmap) {
            const float *uv = m_vbuffer->texcoords[n];
            vt[3] = (m_texture->width()-1)*uv[0]; // u
            vt[4] = (m_texture->height()-1)*uv[1]; // v
        } else {
            vt[3] = 0;
            vt[4] = 0;
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
        size_t n = m_vbuffer->vertices.size;
        assert(n % 3 == 0);
        Vertex vt[3];

        for (size_t i = 0; i < n; i += 3) {
            for (int j = 0; j < 3; j++)
                setVertex(vt[j], i+j);
            drawTriangle(vt);
        }
    }

    void drawTrianglesIndexed()
    {
        size_t n = m_vbuffer->indeces.size;
        Vertex vt[3];

        for (size_t i = 0; i < n; i++) {
            for (int j = 0; j < 3; j++)
                setVertex(vt[j], m_vbuffer->indeces[i][j]);
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

    void texture(const Pixman *texture)
    {
        m_texture = texture;
        m_canvas.texture(texture);
    }

    void vertexBuffer(const VertexBuffer *vb)
    {
        m_vbuffer = vb;
    }

    void wire(bool enable)
    {
        m_wire = enable;
    }

    void reset()
    {
        m_model.loadIdentity();
        m_vbuffer = NULL;
        m_canvas.clear();
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


void testBunny(Renderer &r, float angle)
{
#include "bunny.h"

    r.vertexBuffer(&vb);
    float s = 7.0f;
    r.transform(rotate(angle, 1.f, 1.f, 0.f) * translate(0.f, -0.6f, 0.0f) * scale(s, s, s));
    r.render(TRIANGLES_INDEXED);
}

#define N_ELEMENTS(arr) (sizeof(arr)/sizeof(arr[0]))

void testCube(Renderer &r, float angle)
{
#include "cube.h"

    r.vertexBuffer(&vb);
    float s = 0.3f;
    r.transform(rotate(angle, 1.f, 1.f, 0.f));
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
    float angle = 0.0f;

    bool run = true;
    while (run) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            switch (event.type) {
            case SDL_QUIT:
                run = false;
                break;
            default:
                continue;
            }

        r.reset();
        //testCube(r, angle);
        testBunny(r, angle);
        SDL_Flip(screen);

        angle += 0.01f;
    }

    SDL_Quit();
	return 0;
}
