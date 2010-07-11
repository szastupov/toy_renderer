#ifndef CANVAS_H
#define CANVAS_H

#include "pixman.h"
#include "vec.h"

class Canvas {
    Pixman &m_surface;
    uint32_t m_color;

    void scanlineTriangle(const vec2i v[3], int dir);
public:
    Canvas(Pixman &surf)
        : m_surface(surf)
    {
        color(0xFF, 0x00, 0x00);
    }

    void color(uint8_t r, uint8_t g, uint8_t b);

    void plot(int x, int y);
    void line(int x1, int y1, int x2, int y2);
    void straightLineX(int x1, int x2, int y);
    void straightLineY(int y1, int y2, int x);
    void triangle(const vec2i v[3]);

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
