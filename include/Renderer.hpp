#pragma once

#include <cstdint>

class Renderer
{
public:
    Renderer(
        std::uint32_t* pixels,
        int width,
        int height);

    void setTarget(
        std::uint32_t* pixels); 

    void setPixel(
        int x,
        int y,
        std::uint32_t color); 

    void fill(
        std::uint32_t color);

    void drawRect(
        int x,
        int y,
        int width,
        int height,
        std::uint32_t color);

private:
    std::uint32_t* pixels_;

    int width_;
    int height_;
};