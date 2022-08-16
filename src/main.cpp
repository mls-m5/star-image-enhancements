#include "CImg.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace cimg_library;

using ImgT = CImg<uint8_t>;

uint8_t gray(uint8_t r, uint8_t g, uint8_t b) {
    return (static_cast<uint16_t>(r) + static_cast<uint16_t>(r) +
            static_cast<uint16_t>(r)) /
           3;
}

struct Coord {
    int x = 0;
    int y = 0;

    double dist(Coord other) {
        auto dx = x - other.x;
        auto dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

Coord getMax(const ImgT &img) {
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
std::vector<Coord> getBrightest(ImgT img, int num = 10) {
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

void displayList(ImgT &img, const std::vector<Coord> &coords) {
    auto tc = std::array<uint8_t, 4>{200, 100, 100, 100};

    int i = 0;
    for (auto c : coords) {
        auto [x, y] = c;

        //        img.draw_arrow(x - 10, y - 10, x - 4, y - 4, tc.data(), 1, 30,
        //        4);
        img.draw_text(x, y + 10, std::to_string(i).c_str(), tc.data());

        ++i;
    }
}

struct StarMatch {
    Coord p1;
    Coord p2;
};

std::vector<StarMatch> matchLists(const std::vector<Coord> &c1,
                                  const std::vector<Coord> c2,
                                  double maxDist = 30) {
    auto l2 = c2;
    auto result = std::vector<StarMatch>{};
    for (auto c : c1) {
        auto dist = maxDist;
        int bestMatch = -1;
        for (size_t i = 0; i < l2.size(); ++i) {
            auto d = c.dist(l2.at(i));
            if (d < dist) {
                dist = d;
                bestMatch = i;
            }
        }

        if (bestMatch != -1) {
            result.push_back({c, l2.at(bestMatch)});
            l2.erase(l2.begin() + bestMatch);
        }
    }

    return result;
}

void displayMatches(ImgT &img, const std::vector<StarMatch> &matches) {
    auto tc = std::array<uint8_t, 4>{200, 200, 200, 100};
    for (auto match : matches) {
        img.draw_arrow(match.p1.x,
                       match.p1.y,
                       match.p2.x,
                       match.p2.y,
                       tc.data(),
                       1,
                       30,
                       5);
        img.draw_text(match.p2.x, match.p2.y + 10, "match", tc.data());
    }
}

int main(int argc, char *argv[]) {
    std::cout << "hello there\n";

    auto img = CImg<uint8_t>{};

    img.load_png("testdata/gimp-stars-small.png");

    auto colorImg = img;

    img.resize(img.width(), img.height(), 1);

    auto img2 = img;

    img2.rotate(3);

    img.noise(20, 0);
    img2.noise(20, 0);

    auto pointList = getBrightest(img);
    auto pointList2 = getBrightest(img2);

    //    displayList(colorImg, pointList);
    //    displayList(colorImg, pointList2);

    auto matches = matchLists(pointList, pointList2);
    displayMatches(colorImg, matches);

    auto row1 = ImgT{};

    row1.append(img).append(colorImg);

    auto disp = CImgDisplay{row1};
    while (!disp.is_closed()) {
        disp.wait_all();
    }

    cimg::wait(20);
    return 0;
}
