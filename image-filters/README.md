# Image Filters (C++)

Small C++17 project that loads an image and applies:

1. **Grayscale** (always)
2. A second filter you pick: **sepia**, **invert**, **blur**, or **edges** (Sobel)

Uses [stb_image](https://github.com/nothings/stb) (already included in `third_party/`), so there's nothing to install except a C++ compiler.

## Build

VS Code: `Ctrl+Shift+B`

Or from the terminal:

```
g++ -std=c++17 -O2 -Isrc -Ithird_party src/main.cpp src/filters.cpp -o build/imgfilters
```

Or with CMake:

```
cmake -B build-cmake && cmake --build build-cmake
```

## Run

```
./build/imgfilters images/photo.jpg
```

(on Windows: `.\build\imgfilters.exe images\photo.jpg`)

Run it with no argument and it will ask for the image path. Results are saved in `output/`.

Supported input formats: JPG, PNG, BMP, TGA, GIF, PSD, HDR, PIC, PNM.

## Adding a new filter

1. Declare it in `src/filters.h` and implement it in `src/filters.cpp`
2. Add one line to the `options` list in `src/main.cpp`
