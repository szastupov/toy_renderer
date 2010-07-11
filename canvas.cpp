#include <cstdio>
#include <algorithm>

#include "canvas.h"

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
    if (y1 == y2) {
        if (x1 > x2)
            std::swap(x1, x2);
        straightLineX(x1, x2, y1);
        return;
    }

    if (x1 == x2) {
        if (y1 > y2)
            std::swap(y1, y2);
        straightLineY(y1, y2, x1);
        return;
    }

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    float dx = x2-x1;
    float dy = (y2-y1)/dx;
    float y = y1;

    for (int x = x1; x <= x2; x++) {
        plot(x, y);
        y += dy;
    }
}

void Canvas::straightLineX(int x1, int x2, int y)
{
    for (int x = x1; x <= x2; x++)
        plot(x, y);
}

void Canvas::straightLineY(int y1, int y2, int x)
{
    for (int y = y1; y <= y2; y++)
        plot(x, y);
}

static bool cmpY(const vec2i &a, const vec2i &b)
{
    return a.y() < b.y();
}

// UP_DOWN - for flat bottom
// DOWN_UP - for float top
enum { UP_DOWN, DOWN_UP };

void Canvas::scanlineTriangle(const vec2i v[3], int dir)
{
    int x0, x1, x2;
    float dy;
    float xl, xr;

    if (dir == UP_DOWN) {
        x0 = v[0].x();
        x1 = v[1].x();
        x2 = v[2].x();
        dy = v[2].y() - v[0].y();
        xl = xr = x0;
    } else {
        x0 = v[2].x();
        x1 = v[0].x();
        x2 = v[1].x();
        dy = v[0].y() - v[2].y();
        xl = x1;
        xr = x2;
    }

    if (x1 > x2)
        std::swap(x1, x2);

    float dxl = (x1-x0)/dy;
    float dxr = (x2-x0)/dy;

    for (int y = v[0].y(); y <= v[2].y(); y++) {
        straightLineX(xl, xr, y);
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
    std::copy(vs, vs+3, v);
    std::sort(v, v+3, cmpY);

    if (v[0].y() == v[2].y())
        return;                 // Empty triangle

    if (v[0].y() == v[1].y())
        scanlineTriangle(v, DOWN_UP);
    else if (v[1].y() == v[2].y())
        scanlineTriangle(v, UP_DOWN);
    else {
        // Make two simple triangles
        vec2i vh[3];

        int hy = v[1].y();
        int hx = getX(v[0], v[2], hy);

        vh[0] = v[0];
        vh[1] = v[1];
        vh[2][0] = hx;
        vh[2][1] = hy;
        scanlineTriangle(vh, UP_DOWN);

        vh[0][0] = hx;
        vh[0][1] = hy;
        vh[1] = v[1];
        vh[2] = v[2];
        scanlineTriangle(vh, DOWN_UP);
    }
}
