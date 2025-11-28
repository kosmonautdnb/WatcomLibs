/*
MIT License

Copyright (c) 2018 Volodymyr Bilonenko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
// Stefan Mader prepared it to run on WatcomC++ 11.0 on 26.11.2025
#ifndef __DELAUNTR_HPP__
#define __DELAUNTR_HPP__

#include <math.h>
#include "TYPES.HPP"
#include "ARRAY.HPP"
#include "PAIR.HPP"

#define INLINEX2 __inline

//@see https://stackoverflow.com/questions/33333363/built-in-mod-vs-custom-mod-function-improve-the-performance-of-modulus-op/33333636#33333636
INLINEX2 size_t fast_mod(const size_t i, const size_t c) {
    return i >= c ? i % c : i;
}

double llroundx2(const double k) {
  return floor(k+0.5);
}

// Kahan and Babuska summation, Neumaier variant; accumulates less FP error
INLINEX2 double sum(const Array<double>& x) {
    double sum = x[0];
    double err = 0.0;

    for (size_t i = 1; i < x.size(); i++) {
        const double k = x[i];
        const double m = sum + k;
        err += fabs(sum) >= fabs(k) ? sum - m + k : k - m + sum;
        sum = m;
    }
    return sum + err;
}

INLINEX2 double dist(
    const double ax,
    const double ay,
    const double bx,
    const double by) {
    const double dx = ax - bx;
    const double dy = ay - by;
    return dx * dx + dy * dy;
}

INLINEX2 double circumradius(
    const double ax,
    const double ay,
    const double bx,
    const double by,
    const double cx,
    const double cy) {
    const double dx = bx - ax;
    const double dy = by - ay;
    const double ex = cx - ax;
    const double ey = cy - ay;

    const double bl = dx * dx + dy * dy;
    const double cl = ex * ex + ey * ey;
    const double d = dx * ey - dy * ex;

    const double x = (ey * bl - dy * cl) * 0.5 / d;
    const double y = (dx * cl - ex * bl) * 0.5 / d;

    if ((bl > 0.0 || bl < 0.0) && (cl > 0.0 || cl < 0.0) && (d > 0.0 || d < 0.0)) {
        return x * x + y * y;
    } else {
        return DOUBLE_MAX;
    }
}

INLINEX2 bool orient(
    const double px,
    const double py,
    const double qx,
    const double qy,
    const double rx,
    const double ry) {
    return (qy - py) * (rx - qx) - (qx - px) * (ry - qy) < 0.0;
}

INLINEX2 Pair<double, double> circumcenter(
    const double ax,
    const double ay,
    const double bx,
    const double by,
    const double cx,
    const double cy) {
    const double dx = bx - ax;
    const double dy = by - ay;
    const double ex = cx - ax;
    const double ey = cy - ay;

    const double bl = dx * dx + dy * dy;
    const double cl = ex * ex + ey * ey;
    const double d = dx * ey - dy * ex;

    const double x = ax + (ey * bl - dy * cl) * 0.5 / d;
    const double y = ay + (dx * cl - ex * bl) * 0.5 / d;

    return Pair<double,double>((double)x, (double)y);
}

class DCompare {
public:
    Array<double> const *coords;
    double cx;
    double cy;

    int compare(size_t i, size_t j) {
        const double d1 = dist((*coords)[2 * i], (*coords)[2 * i + 1], cx, cy);
        const double d2 = dist((*coords)[2 * j], (*coords)[2 * j + 1], cx, cy);
        const double diff1 = d1 - d2;
        const double diff2 = (*coords)[2 * i] - (*coords)[2 * j];
        const double diff3 = (*coords)[2 * i + 1] - (*coords)[2 * j + 1];

        if (diff1 > 0.0 || diff1 < 0.0) {
            return diff1 < 0 ? -1 : 1;
        } else if (diff2 > 0.0 || diff2 < 0.0) {
            return diff2 < 0 ? -1 : 1;
        } else {
            return diff3 < 0 ? -1 : diff3 == 0 ? 0 : 1;
        }
        return 0;
    }
};

DCompare dCompare;

INLINEX2 bool in_circle(
    const double ax,
    const double ay,
    const double bx,
    const double by,
    const double cx,
    const double cy,
    const double px,
    const double py) {
    const double dx = ax - px;
    const double dy = ay - py;
    const double ex = bx - px;
    const double ey = by - py;
    const double fx = cx - px;
    const double fy = cy - py;

    const double ap = dx * dx + dy * dy;
    const double bp = ex * ex + ey * ey;
    const double cp = fx * fx + fy * fy;

    return (dx * (ey * cp - bp * fy) -
            dy * (ex * cp - bp * fx) +
            ap * (ex * fy - ey * fx)) < 0.0;
}

const double EPSILON = DOUBLE_EPSILON;
const size_t INVALID_INDEX = SIZE_MAX;

INLINEX2 bool check_pts_equal(double x1, double y1, double x2, double y2) {
    return fabs(x1 - x2) <= EPSILON &&
           fabs(y1 - y2) <= EPSILON;
}

// monotonically increases with real angle, but doesn't need expensive trigonometry
INLINEX2 double pseudo_angle(const double dx, const double dy) {
    const double p = dx / (fabs(dx) + fabs(dy));
    return (dy > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1)
}

struct DelaunatorPoint {
    size_t i;
    double x;
    double y;
    size_t t;
    size_t prev;
    size_t next;
    bool removed;
};

class Delaunator {

public:
    Array<double> const *coords;
    Array<size_t> triangles;
    Array<size_t> halfedges;
    Array<size_t> hull_prev;
    Array<size_t> hull_next;
    Array<size_t> hull_tri;
    size_t hull_start;

    //Delaunator(Array<double> const& in_coords); memory (?fragmentation?) problems with a new instance everytime, so not in constructor :mad:
    Delaunator();
    void delaunator(Array<double> const* in_coords);

    double get_hull_area();

private:
    Array<size_t> m_hash;
    double m_center_x;
    double m_center_y;
    size_t m_hash_size;
    Array<size_t> m_edge_stack;

    size_t legalize(size_t a);
    size_t hash_key(const double x, const double y) const;
    size_t add_triangle(
        size_t i0,
        size_t i1,
        size_t i2,
        size_t a,
        size_t b,
        size_t c);
    void link(const size_t a, const size_t b);
};

template<class T> void swap2x(T &a, T &b) {T c = a; a = (T)b; b = c;}

int dSorter( const void *a, const void *b ) {
  size_t c = *((const size_t*)a);
  size_t d = *((const size_t*)b);
  return dCompare.compare(c,d);
}

Delaunator::Delaunator() {
}

void Delaunator::delaunator(Array<double> const* in_coords) {

    coords = in_coords;
    triangles.clear();
    halfedges.clear();
    hull_prev.clear();
    hull_next.clear();
    hull_tri.clear();
    hull_start=0;
    m_hash.clear();
    m_center_x=0;
    m_center_y=0;
    m_hash_size=0;
    m_edge_stack.clear();


    size_t n = coords->size() >> 1;

    double max_x = DOUBLE_MIN;
    double max_y = DOUBLE_MIN;
    double min_x = DOUBLE_MAX;
    double min_y = DOUBLE_MAX;
    Array<size_t> ids;
    ids.reserve(n);

    {for (size_t i = 0; i < n; i++) {
        const double x = (*coords)[2 * i];
        const double y = (*coords)[2 * i + 1];

        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;

        ids.push_back(i);
    }}
    const double cx = (min_x + max_x) / 2;
    const double cy = (min_y + max_y) / 2;
    double min_dist = DOUBLE_MAX;

    size_t i0 = INVALID_INDEX;
    size_t i1 = INVALID_INDEX;
    size_t i2 = INVALID_INDEX;

    // pick a seed point close to the centroid
    {for (size_t i = 0; i < n; i++) {
        const double d = dist(cx, cy, (*coords)[2 * i], (*coords)[2 * i + 1]);
        if (d < min_dist) {
            i0 = i;
           min_dist = d;
        }
    }}

    const double i0x = (*coords)[2 * i0];
    const double i0y = (*coords)[2 * i0 + 1];

    min_dist = DOUBLE_MAX;

    // find the point closest to the seed
    {for (size_t i = 0; i < n; i++) {
        if (i == i0) continue;
        const double d = dist(i0x, i0y, (*coords)[2 * i], (*coords)[2 * i + 1]);
        if (d < min_dist && d > 0.0) {
            i1 = i;
            min_dist = d;
        }
    }}

    double i1x = (*coords)[2 * i1];
    double i1y = (*coords)[2 * i1 + 1];

    double min_radius = DOUBLE_MAX;

    // find the third point which forms the smallest circumcircle with the first two
    {for (size_t i = 0; i < n; i++) {
        if (i == i0 || i == i1) continue;

        const double r = circumradius(
            i0x, i0y, i1x, i1y, (*coords)[2 * i], (*coords)[2 * i + 1]);

        if (r < min_radius) {
            i2 = i;
            min_radius = r;
        }
    }}

    if (!(min_radius < DOUBLE_MAX)) {
//        throw std::runtime_error("not triangulation");
      return; // no Error check here :mad:
    }

    double i2x = (*coords)[2 * i2];
    double i2y = (*coords)[2 * i2 + 1];

    if (orient(i0x, i0y, i1x, i1y, i2x, i2y)) {
        swap2x(i1, i2);
        swap2x(i1x, i2x);
        swap2x(i1y, i2y);
    }
      
    Pair<double, double> kr = circumcenter(i0x, i0y, i1x, i1y, i2x, i2y);
    m_center_x = kr.first;
    m_center_y = kr.second;

    // sort the points by distance from the seed triangle circumcenter
    dCompare.coords = coords;
    dCompare.cx = m_center_x;
    dCompare.cy = m_center_y;
    qsort(&ids[0],ids.size(),sizeof(size_t),dSorter); // :mad:
    //std::sort(ids.begin(), ids.end(), compare{ coords, m_center_x, m_center_y }); :mad:

    // initialize a hash table for storing edges of the advancing convex hull
    m_hash_size = static_cast<size_t>(llroundx2(ceil(sqrt(n))));
    m_hash.resize(m_hash_size);
    {for (int i = 0; i < m_hash.size(); i++)  m_hash[i] = INVALID_INDEX;}
    //std::fill(m_hash.begin(), m_hash.end(), INVALID_INDEX); :mad:

    // initialize arrays for tracking the edges of the advancing convex hull
    hull_prev.resize(n);
    hull_next.resize(n);
    hull_tri.resize(n);

    hull_start = i0;

    size_t hull_size = 3;

    hull_next[i0] = hull_prev[i2] = i1;
    hull_next[i1] = hull_prev[i0] = i2;
    hull_next[i2] = hull_prev[i1] = i0;

    hull_tri[i0] = 0;
    hull_tri[i1] = 1;
    hull_tri[i2] = 2;

    m_hash[hash_key(i0x, i0y)] = i0;
    m_hash[hash_key(i1x, i1y)] = i1;
    m_hash[hash_key(i2x, i2y)] = i2;

    size_t max_triangles = n < 3 ? 1 : 2 * n - 5;
    triangles.reserve(max_triangles * 3);
    halfedges.reserve(max_triangles * 3);
    add_triangle(i0, i1, i2, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX);
#define QUIETDOUBLENAN 0x00000 // :mad:
    double xp = QUIETDOUBLENAN;
    double yp = QUIETDOUBLENAN;
    for (size_t k = 0; k < n; k++) {
        const size_t i = ids[k];
        const double x = (*coords)[2 * i];
        const double y = (*coords)[2 * i + 1];

        // skip near-duplicate points
        if (k > 0 && check_pts_equal(x, y, xp, yp)) continue;
        xp = x;
        yp = y;

        // skip seed triangle points
        if (
            check_pts_equal(x, y, i0x, i0y) ||
            check_pts_equal(x, y, i1x, i1y) ||
            check_pts_equal(x, y, i2x, i2y)) continue;

        // find a visible edge on the convex hull using edge hash
        size_t start = 0;

        size_t key = hash_key(x, y);
        for (size_t j = 0; j < m_hash_size; j++) {
            start = m_hash[fast_mod(key + j, m_hash_size)];
            if (start != INVALID_INDEX && start != hull_next[start]) break;
        }

        start = hull_prev[start];
        size_t e = start;
        size_t q;

        while (q = hull_next[e], !orient(x, y, (*coords)[2 * e], (*coords)[2 * e + 1], (*coords)[2 * q], (*coords)[2 * q + 1])) { //TODO: does it works in a same way as in JS
            e = q;
            if (e == start) {
                e = INVALID_INDEX;
                break;
            }
        }

        if (e == INVALID_INDEX) continue; // likely a near-duplicate point; skip it

        // add the first triangle from the point
        size_t t = add_triangle(
            e,
            i,
            hull_next[e],
            INVALID_INDEX,
            INVALID_INDEX,
            hull_tri[e]);

        hull_tri[i] = legalize(t + 2);
        hull_tri[e] = t;
        hull_size++;

        // walk forward through the hull, adding more triangles and flipping recursively
        size_t next = hull_next[e];
        while (
            q = hull_next[next],
            orient(x, y, (*coords)[2 * next], (*coords)[2 * next + 1], (*coords)[2 * q], (*coords)[2 * q + 1])) {
            t = add_triangle(next, i, q, hull_tri[i], INVALID_INDEX, hull_tri[next]);
            hull_tri[i] = legalize(t + 2);
            hull_next[next] = next; // mark as removed
            hull_size--;
            next = q;
        }

        // walk backward from the other side, adding more triangles and flipping
        if (e == start) {
            while (
                q = hull_prev[e],
                orient(x, y, (*coords)[2 * q], (*coords)[2 * q + 1], (*coords)[2 * e], (*coords)[2 * e + 1])) {
                t = add_triangle(q, i, e, INVALID_INDEX, hull_tri[e], hull_tri[q]);
                legalize(t + 2);
                hull_tri[q] = t;
                hull_next[e] = e; // mark as removed
                hull_size--;
                e = q;
            }
        }

        // update the hull indices
        hull_prev[i] = e;
        hull_start = e;
        hull_prev[next] = i;
        hull_next[e] = i;
        hull_next[i] = next;

        m_hash[hash_key(x, y)] = i;
        m_hash[hash_key((*coords)[2 * e], (*coords)[2 * e + 1])] = e;
    }

}

double Delaunator::get_hull_area() {
    Array<double> hull_area;
    size_t e = hull_start;
    do {
        hull_area.push_back(((*coords)[2 * e] - (*coords)[2 * hull_prev[e]]) * ((*coords)[2 * e + 1] + (*coords)[2 * hull_prev[e] + 1]));
        e = hull_next[e];
    } while (e != hull_start);
    return sum(hull_area);
}

size_t Delaunator::legalize(size_t a) {
    size_t i = 0;
    size_t ar = 0;
    m_edge_stack.clear();

    // recursion eliminated with a fixed-size stack
    while (true) {
        const size_t b = halfedges[a];

        /* if the pair of triangles doesn't satisfy the Delaunay condition
        * (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
        * then do the same check/flip recursively for the new pair of triangles
        *
        *           pl                    pl
        *          /||\                  /  \
        *       al/ || \bl            al/    \a
        *        /  ||  \              /      \
        *       /  a||b  \    flip    /___ar___\
        *     p0\   ||   /p1   =>   p0\---bl---/p1
        *        \  ||  /              \      /
        *       ar\ || /br             b\    /br
        *          \||/                  \  /
        *           pr                    pr
        */
        const size_t a0 = 3 * (a / 3);
        ar = a0 + (a + 2) % 3;

        if (b == INVALID_INDEX) {
            if (i > 0) {
                i--;
                a = m_edge_stack[i];
                continue;
            } else {
                //i = INVALID_INDEX;
                break;
            }
        }

        const size_t b0 = 3 * (b / 3);
        const size_t al = a0 + (a + 1) % 3;
        const size_t bl = b0 + (b + 2) % 3;

        const size_t p0 = triangles[ar];
        const size_t pr = triangles[a];
        const size_t pl = triangles[al];
        const size_t p1 = triangles[bl];

        const bool illegal = in_circle(
            (*coords)[2 * p0],
            (*coords)[2 * p0 + 1],
            (*coords)[2 * pr],
            (*coords)[2 * pr + 1],
            (*coords)[2 * pl],
            (*coords)[2 * pl + 1],
            (*coords)[2 * p1],
            (*coords)[2 * p1 + 1]);

        if (illegal) {
            triangles[a] = p1;
            triangles[b] = p0;

            auto hbl = halfedges[bl];

            // edge swapped on the other side of the hull (rare); fix the halfedge reference
            if (hbl == INVALID_INDEX) {
                size_t e = hull_start;
                do {
                    if (hull_tri[e] == bl) {
                        hull_tri[e] = a;
                        break;
                    }
                    e = hull_next[e];
                } while (e != hull_start);
            }
            link(a, hbl);
            link(b, halfedges[ar]);
            link(ar, bl);
            size_t br = b0 + (b + 1) % 3;

            if (i < m_edge_stack.size()) {
                m_edge_stack[i] = br;
            } else {
                m_edge_stack.push_back(br);
            }
            i++;

        } else {
            if (i > 0) {
                i--;
                a = m_edge_stack[i];
                continue;
            } else {
                break;
            }
        }
    }
    return ar;
}

INLINEX2 size_t Delaunator::hash_key(const double x, const double y) const {
    const double dx = x - m_center_x;
    const double dy = y - m_center_y;
    return fast_mod(
        static_cast<size_t>(llroundx2(floor(pseudo_angle(dx, dy) * static_cast<double>(m_hash_size)))),
        m_hash_size);
}

size_t Delaunator::add_triangle(
    size_t i0,
    size_t i1,
    size_t i2,
    size_t a,
    size_t b,
    size_t c) {
    size_t t = triangles.size();
    triangles.push_back(i0);
    triangles.push_back(i1);
    triangles.push_back(i2);
    link(t, a);
    link(t + 1, b);
    link(t + 2, c);
    return t;
}

void Delaunator::link(const size_t a, const size_t b) {
    size_t s = halfedges.size();
    if (a == s) {
        halfedges.push_back(b);
    } else if (a < s) {
        halfedges[a] = b;
    } else {
        //throw std::runtime_error("Cannot link edge");
        return; // missing error handling :mad:
    }
    if (b != INVALID_INDEX) {
        size_t s2 = halfedges.size();
        if (b == s2) {
            halfedges.push_back(a);
        } else if (b < s2) {
            halfedges[b] = a;
        } else {
            //throw std::runtime_error("Cannot link edge");
            return; // missing error handling :mad:
        }
    }
}

#endif // __DELAUNTR_HPP__
