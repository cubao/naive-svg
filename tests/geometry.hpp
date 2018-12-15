#pragma once

#include <cmath>
#include <ostream>
#include <vector>

namespace cubao
{
struct Line;

struct Point
{
    double x, y, z;
    Point(double _x = 0.0, double _y = 0.0, double _z = 0.0)
        : x(_x), y(_y), z(_z)
    {
    }
    Point(std::vector<double> xyz)
    {
        x = xyz.size() > 0 ? xyz[0] : 0.0;
        y = xyz.size() > 1 ? xyz[1] : 0.0;
        z = xyz.size() > 2 ? xyz[2] : 0.0;
    }

    Point operator-() const { return {-x, -y, -z}; }
    Point operator-(const Point &p) const
    {
        return {x - p.x, y - p.y, z - p.z};
    }
    Point operator+(const Point &p) const
    {
        return {x + p.x, y + p.y, z + p.z};
    }
    Point operator*(double s) const { return {s * x, s * y, s * z}; }
    Point operator/(double s) const { return {s / x, s / y, s / z}; }

    friend std::ostream &operator<<(std::ostream &out, const Point &p);
};

double dot(const Point &p1, const Point &p2)
{
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

Point cross(const Point &p1, const Point &p2)
{
    double xx = p1.y * p2.z - p2.y * p1.z;
    double yy = p2.x * p1.z - p1.x * p2.z;
    double zz = p1.x * p2.y - p2.x * p1.y;
    return {xx, yy, zz};
}

typedef Point Vector;

struct Line
{
    Point a, b;
    Line(Point _a = Point(), Point _b = Point()) : a(_a), b(_b) {}

    friend std::ostream &operator<<(std::ostream &out, const Line &l);
};

double dist(const Point &p1, const Point &p2)
{
    double dx = p1.x - p2.x;
    double dy = p1.y - p2.y;
    double dz = p1.z - p2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}

double norm(const Point &p)
{
    Point o;
    return dist(p, o);
}

double norm(const Line &l) { return dist(l.a, l.b); }

double dist(const Point &p, const Line &l)
{
    const Point &a = l.a;
    const Point &b = l.b;
    Vector ap = p - a;
    Vector ab = b - a;
    double d = dot(ap, ab);
    double n = norm(l);
    if (d <= 0) {
        return norm(ap);
    }
    if (d / n >= n) {
        return dist(p, b);
    }
    return fabs(norm(cross(ap, ab)) / n);
}
} // namespace cubao
