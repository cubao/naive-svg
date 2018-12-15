#pragma once

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

struct SVG
{
    SVG(double _width=0, double _height=0) : width(_width), height(_height) {
        grid_step = -1;
        grid_color = SVG::Color(-1);
        background = SVG::Color(-1);
    }

    struct Color
    {
        int r, g, b;
        double a;
        Color(int _r = 0, int _g = 0, int _b = 0, double _a = -1)
            : r(_r), g(_g), b(_b), a(_a)
        {
        }
        const static Color RED, GREEN, BLUE, WHITE, BLACK, YELLOW;
        friend std::ostream &operator<<(std::ostream &out, const Color &c);
        bool invalid() const { return r < 0 || g < 0 || b < 0; }
    };

    struct Point
    {
        double x, y;
        Point(double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}
    };

    struct Polyline
    {
        std::vector<Point> points;
        Color stroke;
        double stroke_width;
        Polyline(std::vector<Point> _points, Color _stroke = Color(),
                 double _stroke_width = 1.0)
            : points(_points), stroke(_stroke), stroke_width(_stroke_width)
        {
        }
        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Polyline &p);
    };

    struct Circle
    {
        std::vector<Point> points;
        Point p() const { return points.front(); }
        double r;
        Color stroke, fill;
        double stroke_width;
        Circle(Point _p, double _r, Color _stroke = Color(),
               Color _fill = Color(-1, -1, -1), double _stroke_width = 1.0)
            : r(_r), stroke(_stroke), fill(_fill),
              stroke_width(_stroke_width)
        {
            points.push_back(_p);
            if (fill.invalid()) {
                fill = stroke;
            }
        }

        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Circle &c);
    };

    struct Text
    {
        std::vector<Point> points;
        Point p() const { return points.front(); }
        std::string text;
        Color fill;
        double fontsize;
        Text(Point _p, std::string _text, Color _fill = Color(),
             double _fontsize = 10)
            : text(_text), fill(_fill), fontsize(_fontsize)
        {
            points.push_back(_p);
        }
        friend std::ostream &operator<<(std::ostream &out, const SVG::Text &t);
    };

    void save(std::string path) const;

    double width, height;
    std::vector<Polyline> polylines;
    std::vector<Circle> circles;
    std::vector<Text> texts;

    Color background;
    double grid_step;
    Color grid_color;

    friend std::ostream &operator<<(std::ostream &out, const SVG &s);
};

std::ostream &operator<<(std::ostream &out, const SVG::Color &c);
std::ostream &operator<<(std::ostream &out, const SVG::Polyline &p);
std::ostream &operator<<(std::ostream &out, const SVG::Circle &c);
std::ostream &operator<<(std::ostream &out, const SVG::Text &t);
std::ostream &operator<<(std::ostream &out, const SVG &s);

// implementation

const SVG::Color SVG::Color::RED = SVG::Color(255, 0, 0);
const SVG::Color SVG::Color::GREEN = SVG::Color(0, 255, 0);
const SVG::Color SVG::Color::BLUE = SVG::Color(0, 0, 255);
const SVG::Color SVG::Color::BLACK = SVG::Color(0, 0, 0);
const SVG::Color SVG::Color::WHITE = SVG::Color(255, 255, 255);

void SVG::save(std::string path) const
{
    std::ofstream file(path);
    file << *this;
    file.close();
}

std::ostream &operator<<(std::ostream &out, const SVG::Color &c)
{
    if (0.0 <= c.a && c.a <= 1.0) {
        out << "rgba(" << c.r << "," << c.g << "," << c.b << "," << c.a << ")";
    } else {
        out << "rgb(" << c.r << "," << c.g << "," << c.b << ")";
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG::Polyline &p)
{
    out << "<polyline"                        //
        << " style='stroke:" << p.stroke      //
        << ";stroke-width:" << p.stroke_width //
        << ";fill:none'";
    out << " points='";
    for (auto &pt : p.points) {
        out << pt.x << "," << pt.y << " ";
    }
    out << "'";
    out << " />";
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG::Circle &c)
{
    out << "<circle r='" << c.r << "'"                      //
        << " cx='" << c.p().x << "' cy='" << c.p().y << "'" //
        << " style='stroke:" << c.stroke                    //
        << ";stroke-width:" << c.stroke_width               //
        << ";fill:" << c.fill << "'"                        //
        << " />";
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG::Text &t)
{
    out << "<text"                                        //
        << " x='" << t.p().x << "' y='" << t.p().y << "'" //
        << " fill='" << t.fill << "'"                     //
        << " font-size='" << t.fontsize << "'"            //
        << ">" << t.text << "</text>";
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG &s)
{
    out << "<svg width='" << s.width << "' height='" << s.height << "'>";
    if (!s.background.invalid()) {
        out << "\n\t<rect width='100%' height='100%' fill='" //
            << s.background                                  //
            << "'/>";
    }
    if (s.grid_step > 0) {
        SVG::Color grid_color(155, 155, 155);
        if (!s.grid_color.invalid()) {
            grid_color = s.grid_color;
        }
        for (double i = 0; i < s.height; i += s.grid_step) {
            out << "\n\t" << SVG::Polyline({{0, i}, {s.width, i}}, grid_color);
        }
        for (double j = 0; j < s.width; j += s.grid_step) {
            out << "\n\t" << SVG::Polyline({{j, 0}, {j, s.height}}, grid_color);
        }
    }

    for (auto &p : s.polylines) {
        out << "\n\t" << p;
    }
    for (auto &c : s.circles) {
        out << "\n\t" << c;
    }
    for (auto &t : s.texts) {
        out << "\n\t" << t;
    }
    out << "\n</svg>";
    return out;
}
