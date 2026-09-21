#include "filters.h"

#include <algorithm>
#include <cmath>

static uint8_t clamp8(double v) {
    return static_cast<uint8_t>(std::min(255.0, std::max(0.0, v)));
}

Image toGrayscale(const Image& src) {
    Image out = src;
    for (size_t i = 0; i + 2 < src.data.size(); i += 3) {
        double g = 0.299 * src.data[i] + 0.587 * src.data[i + 1] + 0.114 * src.data[i + 2];
        out.data[i] = out.data[i + 1] = out.data[i + 2] = clamp8(g);
    }
    return out;
}

Image toSepia(const Image& src) {
    Image out = src;
    for (size_t i = 0; i + 2 < src.data.size(); i += 3) {
        double r = src.data[i], g = src.data[i + 1], b = src.data[i + 2];
        out.data[i]     = clamp8(0.393 * r + 0.769 * g + 0.189 * b);
        out.data[i + 1] = clamp8(0.349 * r + 0.686 * g + 0.168 * b);
        out.data[i + 2] = clamp8(0.272 * r + 0.534 * g + 0.131 * b);
    }
    return out;
}

Image toInvert(const Image& src) {
    Image out = src;
    for (auto& v : out.data) v = 255 - v;
    return out;
}

Image toBlur(const Image& src, int radius) {
    Image out = src;
    const int w = src.width, h = src.height;
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double sum[3] = {0, 0, 0};
            int count = 0;
            for (int dy = -radius; dy <= radius; ++dy) {
                for (int dx = -radius; dx <= radius; ++dx) {
                    int nx = std::min(w - 1, std::max(0, x + dx));
                    int ny = std::min(h - 1, std::max(0, y + dy));
                    size_t idx = (static_cast<size_t>(ny) * w + nx) * 3;
                    sum[0] += src.data[idx];
                    sum[1] += src.data[idx + 1];
                    sum[2] += src.data[idx + 2];
                    ++count;
                }
            }
            size_t o = (static_cast<size_t>(y) * w + x) * 3;
            out.data[o]     = clamp8(sum[0] / count);
            out.data[o + 1] = clamp8(sum[1] / count);
            out.data[o + 2] = clamp8(sum[2] / count);
        }
    }
    return out;
}

Image toEdges(const Image& src) {
    Image gray = toGrayscale(src);
    Image out = gray;
    const int w = src.width, h = src.height;
    static const int gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    static const int gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double sx = 0, sy = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int nx = std::min(w - 1, std::max(0, x + kx));
                    int ny = std::min(h - 1, std::max(0, y + ky));
                    double p = gray.data[(static_cast<size_t>(ny) * w + nx) * 3];
                    sx += p * gx[ky + 1][kx + 1];
                    sy += p * gy[ky + 1][kx + 1];
                }
            }
            uint8_t m = clamp8(std::sqrt(sx * sx + sy * sy));
            size_t o = (static_cast<size_t>(y) * w + x) * 3;
            out.data[o] = out.data[o + 1] = out.data[o + 2] = m;
        }
    }
    return out;
}
