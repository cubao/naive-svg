#pragma once

#include <fstream>
#include <ostream>
#include <string>
#include <vector>

namespace cubao
{
struct SVG
{
    SVG(double _width = 0, double _height = 0)
        : width(_width), height(_height), grid_step(-1),
          grid_color(Color::GRAY), background(Color::BLACK)
    {
    }

    struct Color
    {
        int r, g, b;
        double a;
        Color(int _r = 0, int _g = 0, int _b = 0, double _a = -1)
            : r(_r), g(_g), b(_b), a(_a)
        {
        }
        bool invalid() const { return r < 0 || g < 0 || b < 0; }
        friend std::ostream &operator<<(std::ostream &out, const Color &c);

        const static Color RED, GREEN, BLUE, YELLOW, WHITE, GRAY, BLACK;
    };

    struct Element
    {
        std::vector<std::vector<double>> points;
        Color stroke, fill;
        double stroke_width;
        Element() {}
        Element(std::vector<std::vector<double>> _points,
                Color _stroke = Color::BLACK, double _stroke_width = 1,
                Color _fill = Color(-1))
            : points(_points), stroke(_stroke), stroke_width(_stroke_width),
              fill(_fill)
        {
            if (fill.invalid()) {
                fill = stroke;
            }
        }
        double x() const { return points[0][0]; }
        double y() const { return points[0][1]; }
    };

    struct Polyline : Element
    {
        Polyline(std::vector<std::vector<double>> _points,
                 Color _stroke = Color::BLACK, double _stroke_width = 1.0)
            : Element(_points, _stroke, _stroke_width)
        {
        }
        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Polyline &p);
    };

    struct Circle : Element
    {
        double r;
        Circle(std::vector<double> _p, double _r, Color _stroke = Color::BLACK,
               Color _fill = Color(-1), double _stroke_width = 1.0)
            : Element({_p}, _stroke, _stroke_width, _fill), r(_r)
        {
        }
        Circle(double _x, double _y, double _r, Color _stroke = Color::BLACK,
               Color _fill = Color(-1), double _stroke_width = 1.0)
            : Circle({_x, _y}, _r, _stroke, _fill, _stroke_width)
        {
        }

        friend std::ostream &operator<<(std::ostream &out,
                                        const SVG::Circle &c);
    };

    struct Text : Element
    {
        std::string text;
        double fontsize;
        Text(std::vector<double> _p, std::string _text,
             Color _fill = Color::BLACK, double _fontsize = 10)
            : Element({_p}, _fill), text(_text), fontsize(_fontsize)
        {
        }
        Text(double _x, double _y, std::string _text,
             Color _fill = Color::BLACK, double _fontsize = 10)
            : Text({_x, _y}, _text, _fill, _fontsize)
        {
        }

        friend std::ostream &operator<<(std::ostream &out, const SVG::Text &t);
    };

    void save(std::string path) const;

    void fit_to_bbox(double xmin, double xmax, double ymin, double ymax);

    double width, height;
    std::vector<Polyline> polylines;
    std::vector<Circle> circles;
    std::vector<Text> texts;

    double grid_step;
    Color grid_color;
    Color background;

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
const SVG::Color SVG::Color::YELLOW = SVG::Color(255, 255, 0);
const SVG::Color SVG::Color::WHITE = SVG::Color(255, 255, 255);
const SVG::Color SVG::Color::GRAY = SVG::Color(155, 155, 155);
const SVG::Color SVG::Color::BLACK = SVG::Color(0, 0, 0);

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
        out << pt[0] << "," << pt[1] << " ";
    }
    out << "'";
    out << " />";
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG::Circle &c)
{
    out << "<circle r='" << c.r << "'"                  //
        << " cx='" << c.x() << "' cy='" << c.y() << "'" //
        << " style='stroke:" << c.stroke                //
        << ";stroke-width:" << c.stroke_width           //
        << ";fill:" << c.fill << "'"                    //
        << " />";
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG::Text &t)
{
    out << "<text"                                    //
        << " x='" << t.x() << "' y='" << t.y() << "'" //
        << " fill='" << t.fill << "'"                 //
        << " font-size='" << t.fontsize << "'"        //
        << " font-family='monospace'"                 //
        << ">" << t.text << "</text>";
    return out;
}

std::ostream &operator<<(std::ostream &out, const SVG &s)
{
    out << "<svg width='" << s.width << "' height='" << s.height << "'"
        << " xmlns='http://www.w3.org/2000/svg'>";
    if (!s.background.invalid()) {
        out << "\n\t<rect width='100%' height='100%' fill='" //
            << s.background                                  //
            << "'/>";
    }
    if (s.grid_step > 0) {
        SVG::Color grid_color = SVG::Color::GRAY;
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

void SVG::save(std::string path) const
{
    std::ofstream file(path);
    file << *this;
    file.close();
}

void interp(std::vector<std::vector<double>> &points,           //
            double xmin, double xmax, double ymin, double ymax, //
            double width, double height)
{
    double xspan = xmax - xmin;
    double yspan = ymax - ymin;
    for (auto &pt : points) {
        pt[0] = (pt[0] - xmin) / xspan * width;
        pt[1] = (pt[1] - ymin) / yspan * height;
    }
}

void SVG::fit_to_bbox(double xmin, double xmax, double ymin, double ymax)
{
    for (auto &p : polylines) {
        interp(p.points, xmin, xmax, ymin, ymax, width, height);
    }
    for (auto &c : circles) {
        interp(c.points, xmin, xmax, ymin, ymax, width, height);
    }
    for (auto &t : texts) {
        interp(t.points, xmin, xmax, ymin, ymax, width, height);
    }
}
} // namespace cubao
