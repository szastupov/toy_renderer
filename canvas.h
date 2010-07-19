#ifndef CANVAS_H
#define CANVAS_H

#include <limits>
#include "pixman.h"
#include "vec.h"

typedef std::numeric_limits<int32_t> nl32;

struct Vertex {
    float x;
    float y;
    float z;
    float u;
    float v;
};

class Canvas {
    Pixman &m_surface;
    size_t m_zBufferSize;
    int32_t *m_zBuffer;
    const Pixman *m_texture;
    uint32_t m_color;

    void scanlineTriangle(const Vertex v[3], int dir);
public:
    Canvas(Pixman &surf)
        : m_surface(surf)
        , m_zBufferSize(m_surface.width()*m_surface.height())
        , m_zBuffer(new int32_t[m_zBufferSize])
        , m_texture(NULL)
        , m_color(m_surface.mapRGB(0xFF, 0x00, 0x00))
    {
        std::fill(m_zBuffer, m_zBuffer+m_zBufferSize, nl32::max());
    }

    void color(uint8_t r, uint8_t g, uint8_t b);
    void texture(const Pixman *texture)
    {
        m_texture = texture;
    }

    void plot(int x, int y);
    void plot(int x, int y, int z, uint32_t color);
    void line(int x1, int y1, int x2, int y2);
    void straightLineX(int x1, int x2, int y);
    void straightLineY(int y1, int y2, int x);
    void triangle(const Vertex vs[3]);

    int width()
    {
        return m_surface.width();
    }
    int height()
    {
        return m_surface.height();
    }
};

#endif
