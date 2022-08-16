
#include "starlist.h"
#include <glm/geometric.hpp>

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

std::vector<Coord> getBrightest(const ImgT &image, int num) {
    uint8_t black = 0;
    uint8_t white = 255;

    auto pointList = std::vector<Coord>{};

    auto img = image;

    img.blur(5, 5);
    img.normalize(0, 255);

    for (size_t i = 0; i < num; ++i) {

        auto coord = getMax(img);

        pointList.push_back(coord);

        img.draw_circle(coord.x, coord.y, 20, &black);
    }

    return pointList;
}

void displayList(ImgT &img, const std::vector<Coord> &coords) {
    auto tc = std::array<uint8_t, 4>{200, 100, 100, 100};

    int i = 0;
    for (auto c : coords) {
        //        img.draw_arrow(x - 10, y - 10, x - 4, y - 4, tc.data(), 1, 30,
        //        4);
        img.draw_text(c.x, c.y + 10, std::to_string(i).c_str(), tc.data());

        ++i;
    }
}

std::vector<StarMatch> matchLists(const std::vector<Coord> &c1,
                                  const std::vector<Coord> c2,
                                  double maxDist) {
    auto l2 = c2;
    auto result = std::vector<StarMatch>{};
    for (auto c : c1) {
        auto dist = static_cast<double>(maxDist);
        int bestMatch = -1;
        for (size_t i = 0; i < l2.size(); ++i) {
            auto d = glm::length(c - l2.at(i));
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
    }
}

std::vector<StarMatch> getGetMatchingStars(const ImgT &img1, const ImgT &img2) {
    auto pointList = getBrightest(img1);
    auto pointList2 = getBrightest(img2);
    return matchLists(pointList, pointList2);
}
