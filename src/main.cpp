#include "common.h"
#include "starlist.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace cimg_library; //! Retrieve a list of the brightest stars

int main(int argc, char *argv[]) {
    auto img1 = CImg<uint8_t>{};

    img1.load_png("testdata/gimp-stars-small.png");

    auto colorImg = img1;

    img1.resize(img1.width(), img1.height(), 1);

    auto img2 = img1;

    img2.rotate(3);

    // Noise to test robustnes
    img1.noise(20, 0);
    img2.noise(20, 0);

    {
        // This is duplicate and only used for debugging
        auto pointList = getBrightest(img1);
        displayList(colorImg, pointList);
    }

    auto matches = getGetMatchingStars(img1, img2);

    displayMatches(colorImg, matches);

    auto row1 = ImgT{};

    row1.append(img1).append(colorImg);

    auto disp = CImgDisplay{row1};
    while (!disp.is_closed()) {
        disp.wait_all();
    }

    cimg::wait(20);
    return 0;
}
