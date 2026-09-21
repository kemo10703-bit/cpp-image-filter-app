#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#endif

#include "filters.h"

struct FilterOption {
    std::string name;
    std::function<Image(const Image&)> apply;
};

static std::string stripQuotes(std::string s) {
    while (!s.empty() && (s.front() == '"' || s.front() == '\'' || s.front() == ' ')) s.erase(s.begin());
    while (!s.empty() && (s.back() == '"' || s.back() == '\'' || s.back() == ' ')) s.pop_back();
    return s;
}

static void ensureDir(const std::string& dir) {
#ifdef _WIN32
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0755);
#endif
}

static std::string getStem(const std::string& path) {
    size_t slash = path.find_last_of("/\\");
    std::string filename = (slash == std::string::npos) ? path : path.substr(slash + 1);
    size_t dot = filename.find_last_of('.');
    return (dot == std::string::npos) ? filename : filename.substr(0, dot);
}

static bool loadImage(const std::string& path, Image& img) {
    int w, h, c;
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &c, 3);
    if (!pixels) return false;
    img.width = w;
    img.height = h;
    img.data.assign(pixels, pixels + static_cast<size_t>(w) * h * 3);
    stbi_image_free(pixels);
    return true;
}

static bool saveImage(const std::string& path, const Image& img) {
    return stbi_write_png(path.c_str(), img.width, img.height, 3, img.data.data(), img.width * 3) != 0;
}

int main(int argc, char* argv[]) {
    std::string inputPath;
    if (argc > 1) {
        inputPath = stripQuotes(argv[1]);
    } else {
        std::cout << "Image path: ";
        std::getline(std::cin, inputPath);
        inputPath = stripQuotes(inputPath);
    }

    Image src;
    if (!loadImage(inputPath, src)) {
        std::cerr << "Could not load image: " << inputPath << "\n";
        return 1;
    }
    std::cout << "Loaded " << src.width << "x" << src.height << "\n";

    std::vector<FilterOption> options = {
        {"sepia",  toSepia},
        {"invert", toInvert},
        {"blur",   [](const Image& i) { return toBlur(i, 3); }},
        {"edges",  toEdges},
    };

    std::cout << "\nChoose the second filter:\n";
    for (size_t i = 0; i < options.size(); ++i)
        std::cout << "  " << i + 1 << ") " << options[i].name << "\n";
    std::cout << "> ";

    size_t choice = 0;
    std::cin >> choice;
    if (choice < 1 || choice > options.size()) {
        std::cerr << "Invalid choice\n";
        return 1;
    }
    const FilterOption& second = options[choice - 1];

    std::string outDir = "output";
    ensureDir(outDir);
    std::string stem = getStem(inputPath);

    std::string grayOut = outDir + "/" + stem + "_gray.png";
    std::string secondOut = outDir + "/" + stem + "_" + second.name + ".png";

    if (!saveImage(grayOut, toGrayscale(src))) {
        std::cerr << "Failed to save " << grayOut << "\n";
        return 1;
    }
    if (!saveImage(secondOut, second.apply(src))) {
        std::cerr << "Failed to save " << secondOut << "\n";
        return 1;
    }

    std::cout << "\nSaved:\n  " << grayOut << "\n  " << secondOut << "\n";
    return 0;
}