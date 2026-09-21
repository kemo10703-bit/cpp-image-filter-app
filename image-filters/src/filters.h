#pragma once

#include <cstdint>
#include <vector>

struct Image {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> data;
};

Image toGrayscale(const Image& src);
Image toSepia(const Image& src);
Image toInvert(const Image& src);
Image toBlur(const Image& src, int radius = 3);
Image toEdges(const Image& src);
