#include "CImg.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace cimg_library;

uint8_t gray(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint16_t>(r) + static_cast<uint16_t>(r) +
            static_cast<uint16_t>(r)) /
           3;
}

struct Coord {
    int x = 0;
    int y = 0;
};

Coord getMax(const CImg<uint8_t> &img) {
    int maxX = -1, maxY = -1, maxValue = 0;

    cimg_forXY(img, x, y) {
        auto value = img(x, y);
        if (value > maxValue) {
            maxValue = value;
            maxX = x;
            maxY = y;
        }
    }

    return {maxX, maxY};
}

//! Retrieve a list of the brightest stars
std::vector<Coord> getBrightest(CImg<uint8_t> img, int num = 10) {
    uint8_t black = 0;
    uint8_t white = 255;

    auto pointList = std::vector<Coord>{};

    img.blur(5, 5);
    img.normalize(0, 255);

    for (size_t i = 0; i < num; ++i) {

        auto coord = getMax(img);

        pointList.push_back(coord);

        auto [x, y] = coord;

        img.draw_circle(x, y, 20, &black);
    }

    return pointList;
}

int main(int argc, char *argv[]) {
    std::cout << "hello there\n";

    auto img = CImg<uint8_t>{};

    img.load_png("testdata/gimp-stars-small.png");

    auto colorImg = img;

    img.resize(img.width(), img.height(), 1);

    img.noise(20, 0);

    // img.rotate(10);

    auto tc = std::array<uint8_t, 4>{200, 100, 100, 100};

    auto color = 255;

    auto pointList = getBrightest(img);

    int i = 0;
    for (auto c : pointList) {
        auto [x, y] = c;

        colorImg.draw_arrow(x - 10, y - 10, x - 4, y - 4, tc.data(), 1, 30, 4);
        colorImg.draw_text(x, y + 10, std::to_string(i).c_str(), tc.data());

        ++i;
    }

    auto disp = CImgDisplay{
        img,
        "hello",
    };

    auto disp2 = CImgDisplay{
        colorImg,
        "original",
    };

    img.resize(img.width(), img.height(), 1, 3);

    disp.show();
    disp2.show();

    while (!disp.is_closed() && !disp2.is_closed()) {
        disp.wait_all();
    }

    cimg::wait(20);
    return 0;
}
