#include "common.h"
#include "starlist.h"
#include "startransform.h"
#include <glm/glm.hpp>
#include <iostream>

using namespace cimg_library; //! Retrieve a list of the brightest stars

std::tuple<ImgT, ImgT, ImgT> generateTestData() {
    auto img1 = CImg<uint8_t>{};

    img1.load_png("testdata/gimp-stars-small.png");

    auto colorImg = img1;

    img1.resize(img1.width(), img1.height(), 1);

    auto img2 = img1;

    img2.rotate(3);

    // Noise to test robustnes
    img1.noise(20, 0);
    img2.noise(20, 0);

    return {img1, img2, colorImg};
}

ImgT transformImg(const ImgT &img, glm::mat4 mat) {
    auto ret = img;

    ret.fill(0);

    auto inv = glm::inverse(mat);

    cimg_forXY(img, x, y) {
        auto c = glm::vec4{static_cast<float>(x), static_cast<float>(y), 0, 1};
        auto c2 = inv * c;

        cimg_for2C(img, color) {
            uint8_t pixel = 0;
            pixel = img.cubic_atXY_c(c2.x, c2.y, 0, color, pixel);
            ret(x, y, color) = pixel;
        }
    }

    return ret;
}

void display(const ImgT &img1, const ImgT &colorImg) {
    auto row1 = ImgT{};
    row1.append(img1).append(colorImg);

    auto disp = CImgDisplay{row1};
    while (!disp.is_closed()) {
        disp.wait_all();
    }

    cimg::wait(20);
}

// Test following stars and stuff
void test1() {
    auto [img1, img2, colorImg] = generateTestData();

    {
        // This is duplicate and only used for debugging
        auto pointList = getBrightest(img1);
        displayList(colorImg, pointList);
    }

    auto matches = getGetMatchingStars(img1, img2);
    displayMatches(colorImg, matches);

    auto transform =
        findTransform(matches, {img1.width() / 2, img2.height() / 2});

    display(img1, colorImg);
}

void testTransforms() {
    auto img = ImgT{};

    img.load_png("testdata/test-white.png");

    auto mat = glm::identity<glm::mat4>();

    auto fw = static_cast<float>(img.width());
    auto fh = static_cast<float>(img.height());

    mat = glm::translate(mat, {fw / 2, fh / 2, 0});
    mat = glm::rotate(mat, .1f, glm::vec3{0., 0., 1.});
    mat = glm::translate(mat, {-fw / 2, -fh / 2, 0});
    //    mat = glm::translate(mat, {50, 100, 0});

    img = transformImg(img, mat);

    auto disp = CImgDisplay{img};

    while (!disp.is_closed()) {
        disp.wait_all();
    }

    cimg::wait(20);
}

int main(int argc, char *argv[]) {
    testTransforms();

    return 0;
}
