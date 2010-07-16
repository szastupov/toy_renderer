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

// UP_DOWN - for flat bottom
// DOWN_UP - for float top
enum { UP_DOWN, DOWN_UP };

void Canvas::scanlineTriangle(const Vertex vt[3], int dir)
{
    int idx[3];
    int ixl, ixr;

    if (dir == UP_DOWN) {
        idx[0] = 0;
        idx[1] = 1;
        idx[2] = 2;
        ixl = ixr = 0;
    } else {
        idx[0] = 2;
        idx[1] = 0;
        idx[2] = 1;
        ixl = 0;
        ixr = 1;
    }

    float vx[3], vu[3], vv[3];
    for (int i = 0; i < 3; i++) {
        int j = idx[i];
        vx[i] = vt[j].x;
        vu[i] = vt[j].u;
        vv[i] = vt[j].v;
    }

    if (vx[1] > vx[2])
        std::swap(vx[1], vx[2]);

    float dy = vt[idx[2]].y - vt[idx[0]].y,
        dxl = (vx[1]-vx[0])/dy,
        dxr = (vx[2]-vx[0])/dy,
        dudyl = (vu[1]-vu[0])/dy,
        dvdyl = (vv[1]-vv[0])/dy,
        dudyr = (vu[2]-vu[0])/dy,
        dvdyr = (vv[2]-vv[0])/dy;

    float xl = vt[ixl].x,
        xr = vt[ixr].x,
        ul = vt[ixl].u,
        ur = vt[ixr].u,
        vl = vt[ixl].v,
        vr = vt[ixr].v;

    for (int y = vt[0].y; y <= vt[2].y; y++) {
        float dx = xr-xl;
        for (int x = xl; x <= xr; x++)
            plot(x, y);
        xl += dxl;
        xr += dxr;
        ul += dudyl;
        ur += dudyr;
        vl += dvdyl;
        vr += dvdyr;
    }
}

static int getX(const Vertex &a, const Vertex &b, int y)
{
    float dy = b.y-a.y;
    float dx = b.x-a.x;
    return (float)(y-a.y) / (dy/dx) + a.x;
}

static bool cmpY(const Vertex &a, const Vertex &b)
{
    return a.y < b.y;
}

void Canvas::triangle(const Vertex vs[3])
{
    Vertex vt[3];
    std::copy(vs, vs+3, vt);
    std::sort(vt, vt+3, cmpY);

    if (vt[0].y == vt[2].y)
        return;                 // Empty triangle

    if (vt[0].y == vt[1].y)
        scanlineTriangle(vt, DOWN_UP);
    else if (vt[1].y == vt[2].y)
        scanlineTriangle(vt, UP_DOWN);
    else {
        // Make two simple triangles
        Vertex vh[3];

        int hy = vt[1].y;
        int hx = getX(vt[0], vt[2], hy);

        vh[0] = vt[0];
        vh[1] = vt[1];
        vh[2].x = hx;
        vh[2].y = hy;
        scanlineTriangle(vh, UP_DOWN);

        vh[0].x = hx;
        vh[0].y = hy;
        vh[1] = vt[1];
        vh[2] = vt[2];
        scanlineTriangle(vh, DOWN_UP);
    }
}
