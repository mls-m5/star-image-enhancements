#include "common.h"
#include "starlist.h"
#include "startransform.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace cimg_library; //! Retrieve a list of the brightest stars

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

std::tuple<std::vector<ImgT>, ImgT> generateTestData(float rotation,
                                                     glm::vec2 translation) {

    auto colorImg = ImgT{};
    colorImg.load_png("testdata/gimp-stars-small.png");

    auto img1 = colorImg;

    auto imgs = std::vector<ImgT>{colorImg};

    imgs.reserve(10);

    float fw = img1.width();
    float fh = img1.height();

    auto mat = glm::identity<glm::mat4>();
    for (size_t i = 0; i < 2; ++i) {
        mat = glm::translate(mat, {fw / 2, fh / 2, 0});
        mat = glm::rotate(mat, rotation, {0, 0, 1});
        mat = glm::translate(mat, {-fw / 2, -fh / 2, 0});
        mat = glm::translate(mat, {translation, 0});
        auto img2 = transformImg(img1, mat);
        img2.noise(20, 0);
        imgs.emplace_back(std::move(img2));
    }

    // Noise to test robustnes
    imgs.front().noise(20, 0);

    return {imgs, colorImg};
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

void drawMatrix(ImgT &img, glm::mat4 mat) {
    float w = img.width() - 1;
    float h = img.height() - 1;

    auto p1 = mat * glm::vec4{0, 0, 0, 1};
    auto p2 = mat * glm::vec4{w, 0, 0, 1};
    auto p3 = mat * glm::vec4{w, h, 0, 1};
    auto p4 = mat * glm::vec4{0, h, 0, 1};

    auto dl = [&img](auto p1, auto p2) {
        uint8_t color[] = {200, 200, 200};
        img.draw_line(p1.x, p1.y, p2.x, p2.y, color);
    };

    dl(p1, p2);
    dl(p2, p3);
    dl(p3, p4);
    dl(p4, p1);

    dl(p1, p3);
    dl(p2, p4);
}

// Test following stars and stuff
void test1() {
    auto [imgs, colorImg] = generateTestData(+.04, {3, 3});

    {
        // This is duplicate and only used for debugging
        auto pointList = getBrightest(imgs.at(0));
        displayList(colorImg, pointList);
    }

    auto displayTransform = glm::identity<glm::mat4>();

    for (size_t i = 1; i < imgs.size(); ++i) {
        auto matches = getGetMatchingStars(imgs.at(i - 1), imgs.at(i));
        displayMatches(colorImg, matches);

        auto transform = findTransform(
            matches, {imgs.at(i).width() / 2, imgs.at(i).height() / 2});

        displayTransform *= transform;

        drawMatrix(colorImg, displayTransform);
    }

    display(imgs.at(1), colorImg);
}

void testDrawTransforms() {
    auto img = ImgT{};

    img.load_png("testdata/test-white.png");

    auto mat = glm::identity<glm::mat4>();

    auto fw = static_cast<float>(img.width());
    auto fh = static_cast<float>(img.height());

    mat = glm::translate(mat, {fw / 2, fh / 2, 0});
    mat = glm::rotate(mat, .1f, glm::vec3{0., 0., 1.});
    mat = glm::translate(mat, {-fw / 2, -fh / 2, 0});

    img = transformImg(img, mat);

    auto disp = CImgDisplay{img};

    while (!disp.is_closed()) {
        disp.wait_all();
    }

    cimg::wait(20);
}

int main(int argc, char *argv[]) {

    test1();

    return 0;
}
