
#include "starmatch.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <vector>

glm::mat4x4 findTransform(const MatchList &list,
                          glm::vec2 center,
                          float learningRate = .1) {
    auto transform = glm::mat4x4{};

    {
        float angle = 0;
        float tx = 0;
        float ty = 0;

        for (auto c : list) {
            auto d = c.p1 - c.p2;

            auto fromCenter = c.p1 - center;

            angle += glm::cross(d, fromCenter) / 10.;
            tx += d.x;
            ty += d.y;
        }
        transform = glm::translate(transform, {-center.x, -center.y, 0});
        transform =
            glm::rotate(transform, angle * learningRate, glm::vec3{0., 0., 1.});
        transform = glm::translate(transform, {center.x, center.y, 0});
    }

    return transform;
}
