#include <algorithm>

#include "canvas.h"

#define SWAP(x, y) { x ^= y; y ^= x; x ^= y; }

void Canvas::color(uint8_t r, uint8_t g, uint8_t b)
{
    m_color = m_surface.mapRGB(r, g, b);
}

void Canvas::plot(int x, int y)
{
    if (x < 0 || x > m_surface.width() ||
        y < 0 || y > m_surface.height())
        return;

    m_surface.set(x, y, m_color);
}

void Canvas::line(int x1, int y1, int x2, int y2)
{
    bool steep = abs(y2 - y1) > abs(x2 - x1);
    if (steep) {
        SWAP(x1, y1);
        SWAP(x2, y2);
    }
    if (x1 > x2) {
        SWAP(x1, x2);
        SWAP(y1, y2);
    }

    int dx = x2-x1,
        dy = abs(y2-y1),
        error = dx >> 1,
        ystep = y1 < y2 ? 1 : -1;

    for (int x = x1, y = y1; x <= x2; x++) {
        if (steep)
            plot(y, x);
        else
            plot(x, y);
        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

static int getX(const vec2i &a, const vec2i &b, int y)
{
    vec2i d = b-a;
    return (float)(y-a.y()) / ((float)d.y()/(float)d.x()) + a.x();
}

static bool cmpY(const vec2i &a, const vec2i &b)
{
    return a.y() < b.y();
}

void Canvas::triangle(const vec2i vs[3])
{
    vec2i v[3];
    for (int i = 0; i < 3; i++)
        v[i] = vs[i];
    std::sort(v, v+3, cmpY);

    if (v[0].y() == v[2].y())
        return;                 // Empty triangle

    int x1, x2;
    int h;                      // 0 - top half, 1- bottom
    for (int y = v[0].y(); y <= v[2].y(); y++) {
        h = y < v[1].y() ? 0 : 1;

        x1 = getX(v[0], v[2], y);
        if (v[h].y() == v[h+1].y())
            x2 = v[h+1].x();
        else
            x2 = getX(v[h], v[h+1], y);

        if (x1 > x2)
            SWAP(x2, x1);
        for (int x = x1; x <= x2; x++)
            plot(x, y);
    }
}
