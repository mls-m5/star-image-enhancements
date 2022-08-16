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

// TODO: Fix... Does not seem to work for whataever reason
void applyTransformedImg(CImg<uint32_t> &target,
                         const ImgT &source,
                         glm::mat4 mat) {
    auto inv = glm::inverse(mat);

    cimg_forXY(target, x, y) {
        auto c = glm::vec4{static_cast<float>(x), static_cast<float>(y), 0, 1};
        auto c2 = inv * c;

        cimg_for2C(target, color) {
            uint32_t pixel = 0;
            pixel = source.cubic_atXY_c(c2.x, c2.y, 0, color, pixel);
            target(x, y, color) += pixel;
            //            target(x, y, color) = pixel;
        }
    }

    //    std::cout << target(10, 10, 0) << std::endl;
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

void display(const ImgT &img1, const ImgT &colorImg, const ImgT &result) {
    auto row1 = ImgT{};
    row1.append(img1).append(colorImg);

    auto row2 = ImgT{};
    row2.append(result, 'x');

    auto disp = CImgDisplay{row1};
    auto disp2 = CImgDisplay{row2};
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
    auto transforms = std::vector{glm::identity<glm::mat4>()};

    for (size_t i = 1; i < imgs.size(); ++i) {
        auto matches = getGetMatchingStars(imgs.at(i - 1), imgs.at(i));
        displayMatches(colorImg, matches);

        auto transform = findTransform(
            matches, {imgs.at(i).width() / 2, imgs.at(i).height() / 2});

        // TODO: run find-transform from the first image to every following
        // image

        transforms.push_back(transform);

        displayTransform *= transform;

        drawMatrix(colorImg, displayTransform);
    }

    auto result32 = CImg<uint32_t>{};
    result32.resize(colorImg.width(), colorImg.height(), 3);

    {
        auto cumulativeTransform = glm::identity<glm::mat4>();

        for (size_t i = 0; i < imgs.size(); ++i) {
            cumulativeTransform *= transforms.at(i);
            auto tmp =
                transformImg(imgs.at(i), glm::inverse(cumulativeTransform));
            result32 += tmp;
        }
    }

    auto result = ImgT{};
    result.resize(result32.width(), result32.height(), 3);
    for (size_t i = 0; i < result.size(); ++i) {
        result.at(i) = result32.at(i) / imgs.size();
    }

    display(imgs.at(1), colorImg, result);
}

void testDrawTransforms() {
    auto img = ImgT{};

    img.load_png("testdata/test-white.png");

    //    img.resize(-100, -100, 1);

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

    switch (0) {
    case 0:
        test1();
        break;
    case 1:
        testDrawTransforms();
        break;
    }

    return 0;
}
