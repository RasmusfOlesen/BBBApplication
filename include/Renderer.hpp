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

    void drawLine(
        int x0,
        int y0,
        int x1,
        int y1,
        int thickness,
        std::uint32_t color);

    void drawCircle(
        int centerX,
        int centerY,
        int radius,
        int thickness,
        std::uint32_t color);

    void fillCircle(
        int centerX,
        int centerY,
        int radius,
        std::uint32_t color);

    void drawArc(
        int centerX,
        int centerY,
        int radius,
        double startAngle,
        double endAngle,
        int thickness,
        std::uint32_t color);

private:
    std::uint32_t* pixels_;

    int width_;
    int height_;
};