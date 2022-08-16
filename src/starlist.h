#pragma once

#include "common.h"
#include <vector>

// TODO: make coordinates double, and interpolate between pixels
struct Coord {
    int x = 0;
    int y = 0;

    double dist(Coord other) {
        auto dx = x - other.x;
        auto dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

struct StarMatch {
    Coord p1;
    Coord p2;
};

// Get the brightest pixels on a image
std::vector<Coord> getBrightest(const ImgT &img, int num = 10);

void displayList(ImgT &img, const std::vector<Coord> &coords);

std::vector<StarMatch> matchLists(const std::vector<Coord> &c1,
                                  const std::vector<Coord> c2,
                                  double maxDist = 30);

void displayMatches(ImgT &img, const std::vector<StarMatch> &matches);

std::vector<StarMatch> getGetMatchingStars(const ImgT &img1, const ImgT &img2);
