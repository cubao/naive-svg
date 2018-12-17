#include "geometry.hpp"
#include "svg.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

using namespace std;
using namespace std::chrono;
using namespace cubao;

size_t unix_time();

vector<vector<double>> douglas(const vector<vector<double>> &points,
                               double thresh);

void update_svg(SVG &svg, const vector<vector<double>> &points, //
                SVG::Color line_color, double line_width,       //
                SVG::Color pt_color, double point_radius);

double rand01();

vector<double> next_random_point(const vector<double> &_p0,
                                 const vector<double> &_p1, double min_step,
                                 double madness);

void bbox(const vector<vector<double>> &points, //
          double &xmin, double &xmax, double &ymin, double &ymax);

int main(int argc, char **argv)
{
    srand(time(nullptr));
    cout << "Usage:\n\t" << argv[0] << "[npoint] [madness] [min_step] [thresh]"
         << endl;

    int n_points = 20;
    if (argc > 1) {
        n_points = atoi(argv[1]);
    }
    double madness = 0.5;
    if (argc > 2) {
        madness = atof(argv[2]);
    }
    double min_step = 50.0;
    if (argc > 3) {
        min_step = atof(argv[3]);
    }
    double thresh = 20;
    if (argc > 4) {
        thresh = atof(argv[4]);
    }
    cout << "n_points: " << n_points << endl;
    cout << "madness: " << madness << endl;
    cout << "min_step: " << min_step << endl;
    cout << "thresh: " << thresh << endl;

    SVG svg;
    double &width = svg.width;
    double &height = svg.height;
    svg.grid_step = 10;
    svg.grid_color = SVG::Color::GRAY;
    svg.background = SVG::Color::WHITE;

    vector<vector<double>> points{{0, 0}, {0, min_step}};
    for (int i = 0; i < n_points; i++) {
        points.push_back(next_random_point(points[points.size() - 2],
                                           points.back(), min_step, madness));
    }
    double xmin, xmax, ymin, ymax;
    xmin = xmax = points[0][0];
    ymin = ymax = points[0][1];
    bbox(points, xmin, xmax, ymin, ymax);

    auto points_daug = douglas(points, thresh);
    update_svg(svg, points, SVG::Color(), 1, SVG::Color::RED, 3);
    update_svg(svg, points_daug, SVG::Color(), 3, SVG::Color::GREEN, 5);

    double border_width = 20;
    xmin -= border_width;
    xmax += border_width;
    ymin -= border_width;
    ymax += border_width;

    stringstream ss;
    ss << "#points: " << points.size() << " -> " << points_daug.size();
    cout << ss.str() << endl;
    svg.texts.push_back(
        SVG::Text({xmin, ymax - 10}, ss.str(), SVG::Color::RED, 24));

    width = xmax - xmin;
    height = ymax - ymin;
    double ratio = height / width;
    if (width > height) {
        width = 1000;
        height = 1000 * ratio;
    } else {
        height = 1000;
        width = height / ratio;
    }

    cout << "width: " << width << " height: " << height << endl;

    svg.fit_to_bbox(xmin, xmax, ymin, ymax);

    size_t epoch = unix_time();
    string path = "test_douglas_" + to_string(epoch) + ".svg";
    svg.save(path);
    cout << "wrote to '" << path << "'" << endl;
    return 0;
}

void update_svg(SVG &svg, const vector<vector<double>> &points, //
                SVG::Color line_color, double line_width,       //
                SVG::Color pt_color, double point_radius)
{
    for (auto &pt : points) {
        svg.circles.push_back(SVG::Circle(pt, point_radius, pt_color));
    }
    svg.polylines.push_back(SVG::Polyline(points, line_color, line_width));
}

vector<vector<double>> douglas(const vector<vector<double>> &points,
                               double thresh)
{
    if (points.size() <= 2) {
        return points;
    }
    Line line(points.front(), points.back());
    int max_index = 1;
    double max_dist = dist(points[max_index], line);
    for (int i = 2; i < points.size() - 1; i++) {
        double d = dist(points[i], line);
        if (d > max_dist) {
            max_dist = d;
            max_index = i;
        }
    }
    if (max_dist < thresh) {
        return {points.front(), points.back()};
    }
    auto lefts = douglas(
        vector<vector<double>>(points.begin(), points.begin() + max_index + 1),
        thresh);
    auto rights = douglas(
        vector<vector<double>>(points.begin() + max_index, points.end()),
        thresh);
    vector<vector<double>> ret;
    copy(lefts.begin(), lefts.end(), back_inserter(ret));
    copy(rights.begin() + 1, rights.end(), back_inserter(ret));
    return ret;
}

size_t unix_time()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

double rand01() { return rand() / (double)RAND_MAX; }

vector<double> next_random_point(const vector<double> &_p0,
                                 const vector<double> &_p1, double min_step,
                                 double madness)
{
    Point p0(_p0);
    Point p1(_p1);
    Point p01 = p1 - p0;
    double rad = rand01() * 360 / 180.0 * 3.14;
    double radius = madness * rand01() * norm(p01);
    Point delta = p01 + Point(cos(rad) * radius, sin(rad) * radius);
    double scale = max(min_step / norm(delta), 1.0);
    Point pp = p1 + delta * scale;
    return {pp.x, pp.y, pp.z};
}

void bbox(const vector<vector<double>> &points, //
          double &xmin, double &xmax, double &ymin, double &ymax)
{
    for (auto &pt : points) {
        xmin = min(xmin, pt[0]);
        xmax = max(xmax, pt[0]);
        ymin = min(ymin, pt[1]);
        ymax = max(ymax, pt[1]);
    }
}