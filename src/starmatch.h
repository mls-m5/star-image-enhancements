#pragma once

#include <glm/vec2.hpp>
#include <vector>

using Coord = glm::vec2;

struct StarMatch {
    Coord p1;
    Coord p2;
};

using MatchList = std::vector<StarMatch>;
