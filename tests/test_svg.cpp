#include "svg.hpp"
#include "encodeURIComponent.hpp"

#include <chrono>
#include <iostream>
#include <sstream>

using namespace std;
using namespace std::chrono;
using namespace cubao;

size_t unix_time()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
        .count();
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        string encoded = svg2dataUrl(argv[1]);
        cout << "dataurl: (paste to browerser navigation to view)\n"
            << encoded << endl;
        return 0;
    }
    SVG svg(40, 30);
    svg.circles.push_back(
        SVG::Circle({10, 10}, 4, SVG::Color::BLACK, SVG::Color::GREEN));
    svg.texts.push_back(SVG::Text({8, 6}, "some text", SVG::Color::RED, 8));
    svg.polylines.push_back(SVG::Polyline({
        {1.1, 2.2}, {5.1, 3.2}, {8.1, 9.2},
    }));
    svg.polygons.push_back(SVG::Polygon({{6.1, 8.2}, {11.2, 3}, {13.1, 14.2}},
                                        SVG::Color::RED, 0.5,
                                        SVG::Color(255, 255, 0, 0.5)));

    stringstream ss;
    ss << svg << endl;
    cout << svg << endl;

    size_t epoch = unix_time();
    string path = to_string(epoch) + ".svg";
    svg.save(path);
    cout << "wrote to '" << path << "'" << endl;
    return 0;
}
