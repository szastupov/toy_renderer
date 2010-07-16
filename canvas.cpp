#include <cstdio>
#include <algorithm>
#include <cassert>

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

void Canvas::plot(int x, int y, uint32_t color)
{
    if (x < 0 || x > m_surface.width() ||
        y < 0 || y > m_surface.height())
        return;

    m_surface.set(x, y, color);
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
    static const int idx1[3] = { 0, 1, 2};
    static const int idx2[3] = { 2, 0, 1};
    const int *idx;
    int l0, r0;

    if (dir == UP_DOWN) {
        idx = idx1;
        l0 = r0 = 0;
    } else {
        idx = idx2;
        l0 = 1;
        r0 = 2;
    }

    vec3f v[3];
    for (int i = 0; i < 3; i++) {
        int j = idx[i];
        v[i][0] = vt[j].x;
        v[i][1] = vt[j].u;
        v[i][2] = vt[j].v;
    }

    if (v[1].x() > v[2].x())
        std::swap(v[1][0], v[2][0]);

    float dy = vt[idx[2]].y - vt[idx[0]].y;
    vec3f dvl = (v[1]-v[0])/dy;
    vec3f dvr = (v[2]-v[0])/dy;
    vec3f vl = v[l0];
    vec3f vr = v[r0];

    for (int y = vt[0].y; y <= vt[2].y; y++) {
        vec3f ddv = vr-vl;
        vec2f duv(ddv[1], ddv[2]);
        duv /= ddv.x();
        vec2f uv(vl[1], vl[2]);
        for (int x = vl[0]; x <= vr[0]; x++) {
            plot(x, y, m_texture->get(uv[0], uv[1]));
            uv += duv;
        }
        vl += dvl;
        vr += dvr;
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
