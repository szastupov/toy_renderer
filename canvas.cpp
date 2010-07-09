#include <cstdio>
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

void Canvas::straightLine(int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x++)
        plot(x, y);
}

static bool cmpY(const vec2i &a, const vec2i &b)
{
    return a.y() < b.y();
}

// static vec2i getDimensions(const vec2i v[3])
// {
//     int minx = v[0].x();
//     int maxx = v[0].x();
//     for (int i = 1; i < 3; i++) {
//         minx = std::min(minx, v[i].x());
//         maxx = std::max(maxx, v[i].x());
//     }
//     int width = maxx - minx;
//     int height = v[2].y() - v[0].y();

//     return vec2i(width, height);
// }


void Canvas::triangleUpDown(vec2i v[3])
{
    int x0 = v[0].x();
    int x1 = std::min(v[1].x(), v[2].x());
    int x2 = std::max(v[1].x(), v[2].x());

    float dy = v[2].y() - v[0].y();
    float dxl = (x1 - x0)/dy;
    float dxr = (x2 - x0)/dy;
    float xl = x0;
    float xr = xl;

    for (int y = v[0].y(); y <= v[2].y(); y++) {
        straightLine(xl, xr, y);
        xl += dxl;
        xr += dxr;
    }
}


void Canvas::triangleDownUp(vec2i v[3])
{
    int x0 = v[2].x();
    int x1 = std::min(v[0].x(), v[1].x());
    int x2 = std::max(v[0].x(), v[1].x());

    float dy = v[2].y() - v[0].y();
    float dxl = (x1 - x0)/dy;
    float dxr = (x2 - x0)/dy;
    float xl = x0;
    float xr = xl;

    for (int y = v[2].y(); y >= v[0].y(); y--) {
        straightLine(xl, xr, y);
        xl += dxl;
        xr += dxr;
    }
}

static int getX(const vec2i &a, const vec2i &b, int y)
{
    vec2i d = b-a;
    return (float)(y-a.y()) / ((float)d.y()/(float)d.x()) + a.x();
}

void Canvas::triangle(const vec2i vs[3])
{
    vec2i v[3];
    for (int i = 0; i < 3; i++)
        v[i] = vs[i];
    std::sort(v, v+3, cmpY);

    if (v[0].y() == v[2].y())
        return;                 // Empty triangle

    if (v[0].y() == v[1].y())
        triangleDownUp(v);
    else if (v[1].y() == v[2].y())
        triangleUpDown(v);
    else {
        vec2i vh[3];

        int hy = v[1].y();
        int hx = getX(v[0], v[2], hy);

        vh[0] = v[0];
        vh[1] = v[1];
        vh[2][0] = hx;
        vh[2][1] = hy;
        triangleUpDown(vh);

        vh[0][0] = hx;
        vh[0][1] = hy;
        vh[1] = v[1];
        vh[2] = v[2];
        triangleDownUp(vh);
    }
}
