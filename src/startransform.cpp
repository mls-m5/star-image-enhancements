#include "startransform.h"
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/exterior_product.hpp>
#include <glm/vec2.hpp>
#include <iostream>
#include <vector>

glm::mat4x4 findTransform(MatchList list,
                          glm::vec2 center,
                          float learningRate) {
    auto transform = glm::identity<glm::mat4x4>();

    auto partialErrors = std::vector<float>(list.size());

    auto iteration = [&list, &transform, &partialErrors, learningRate, center] {
        float angle = 0;
        float tx = 0;
        float ty = 0;

        float error = 0;

        std::cout << "partial errors" << std::endl;

        for (size_t listIndex = 0; listIndex < list.size(); ++listIndex) {
            auto c = list.at(listIndex);

            auto np = transform * glm::vec4{c.p1, 0, 1};
            auto d = glm::vec2{np} - c.p2;

            auto e = glm::length(d);
            partialErrors.at(listIndex) = e;

            std::cout << " " << e;

            error += e;

            auto fromCenter = c.p1 - center;

            angle += glm::cross(d, fromCenter) / 1000.;
            tx -= d.x;
            ty -= d.y;
        }
        std::cout << "\n";

        error /= list.size();

        std::cerr << "error: " << error << std::endl;

        transform = glm::translate(transform, {center.x, center.y, 0});
        transform = glm::rotate(transform,
                                angle * learningRate / list.size(),
                                glm::vec3{0., 0., 1.});
        transform = glm::translate(transform, {-center.x, -center.y, 0});
        transform = glm::translate(transform,
                                   {tx * learningRate, ty * learningRate, 0});
    };

    for (size_t i = 0; i < 100; ++i) {
        iteration();
    }

    auto itMin = std::min_element(partialErrors.begin(), partialErrors.end());
    auto itMax = std::max_element(partialErrors.begin(), partialErrors.end());

    std::cout << "middle check\n";

    std::cout << "min partial error " << *itMin << "\n";
    std::cout << "max partial error " << *itMax << "\n";

    auto removeWorst = [&list, &partialErrors](float threshold) -> bool {
        auto itMax =
            std::max_element(partialErrors.begin(), partialErrors.end());

        if (*itMax < threshold) {
            return false;
        }

        partialErrors.erase(itMax);
        auto i = std::distance(partialErrors.begin(), itMax);
        list.erase(list.begin() + i);

        return true;
    };

    for (; removeWorst(*itMin * 3);)
        ;

    for (size_t i = 0; i < 100; ++i) {
        iteration();
    }

    return transform;
}
