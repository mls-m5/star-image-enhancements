#pragma once

#include "common.h"
#include "starmatch.h"

// Get the brightest pixels on a image
std::vector<Coord> getBrightest(const ImgT &img, int num = 10);

void displayList(ImgT &img, const std::vector<Coord> &coords);

std::vector<StarMatch> matchLists(const std::vector<Coord> &c1,
                                  const std::vector<Coord> c2,
                                  double maxDist = 30);

void displayMatches(ImgT &img, const std::vector<StarMatch> &matches);

std::vector<StarMatch> getGetMatchingStars(const ImgT &img1, const ImgT &img2);
