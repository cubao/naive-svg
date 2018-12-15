#include "svg.hpp"

#include <iostream>
#include <sstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

size_t unix_time() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

int main(void)
{
    SVG svg(40, 30);
    svg.circles.push_back(SVG::Circle(SVG::Point(10, 10), 4, SVG::Color::BLACK,
                                      SVG::Color::GREEN));
    svg.texts.push_back(
        SVG::Text(SVG::Point(8, 6), "some text", SVG::Color::RED, 8));
    svg.polylines.push_back(SVG::Polyline({
        {1.1, 2.2}, {5.1, 3.2}, {8.1, 9.2},
    }));

    stringstream ss;
    ss << svg << endl;
    cout << svg << endl;

    size_t epoch = unix_time();
    string path = to_string(epoch) + ".svg";
    svg.save(path);
    cout << "wrote to '" << path << "'" << endl;
    return 0;
}
