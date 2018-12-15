#include "svg.hpp"

#include <iostream>
#include <sstream>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace std::chrono;

size_t unix_time() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

vector<vector<double>> douglas(const vector<vector<double>> &points,
                               double thresh = 5);

void update_svg(SVG &svg, const vector<vector<double>> &points, //
                SVG::Color line_color, double line_width,       //
                SVG::Color pt_color, double point_radius);

int main(void)
{
    SVG svg(800, 600);
    svg.grid_step = 10;
    svg.grid_color = SVG::Color(155, 155, 155);
    svg.background = SVG::Color::WHITE;

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
    vector<SVG::Point> polyline_points;
    transform(
        points.begin(), points.end(), back_inserter(polyline_points),
        [](const vector<double> &pt) { return SVG::Point(pt[0], pt[1]); });
    for (auto &pt : polyline_points) {
        svg.circles.push_back(
            SVG::Circle({pt.x, pt.y}, point_radius, pt_color));
        svg.width = max(svg.width, pt.x + 20);
        svg.height = max(svg.height, pt.y + 20);
    }
    svg.polylines.push_back(
        SVG::Polyline(polyline_points, line_color, line_width));
}

double dist(const vector<double> &p, // point p to line ab
            const vector<double> &a, const vector<double> &b)
{
}


vector<vector<double>> douglas(const vector<vector<double>> &points,
                               double thresh)
{
    if (points.size() <= 2) {
        return points;
    }
    Line line(points.front(), points.back());
    int max_index = 1;
    double max_dist = line.distTo(points[max_index]);
    for (int i = 2; i < points.size() - 1; i++) {
        double dist = line.distTo(points[i]);
        if (dist > max_dist) {
            max_dist = dist;
            max_index = i;
        }
    }
    if (max_dist < thresh) {
        return {points.front(), points.back()};
    }
    vector<Point> lefts = douglas(
        vector<Point>(points.begin(), points.begin() + max_index + 1), thresh);
    vector<Point> rights = douglas(
        vector<Point>(points.begin() + max_index, points.end()), thresh);
    vector<Point> ret;
    copy(lefts.begin(), lefts.end(), back_inserter(ret));
    copy(rights.begin() + 1, rights.end(), back_inserter(ret));
    return ret;
}
