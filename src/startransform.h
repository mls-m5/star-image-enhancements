
#include "debug.h"
#include "starmatch.h"
#include <glm/mat4x4.hpp>

glm::mat4x4 findTransform(MatchList list,
                          glm::vec2 center,
                          float learningRate = .02);
