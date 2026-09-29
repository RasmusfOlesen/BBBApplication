#pragma once

#include "TextRenderer.hpp"

#include <vector>
#include <cstdint>
#include <unordered_map>

enum class LineCap
{
    Butt,
    Square,
    Round
};

struct Point
{
    int x;
    int y;
};

using RegionId = std::uint32_t;

struct RenderRegion
{
    int x;
    int y;
    int width;
    int height;

    std::vector<std::uint32_t> pixels;
};

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

    void drawPolygon(
        const std::vector<Point>& points,
        std::uint32_t color);

    void drawLine(
        int x0,
        int y0,
        int x1,
        int y1,
        int thickness,
        std::uint32_t color,
        LineCap = LineCap::Butt);

    void drawRotatedLine(
        int startX,
        int startY,
        int endX,
        int endY,
        int originX,
        int originY,
        double angle,
        int thickness,
        std::uint32_t color,
        LineCap = LineCap::Butt);

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
        std::uint32_t color,
        LineCap = LineCap::Butt);

    void drawText(
        const char* text,
        int x,
        int y,
        int size,
        std::uint32_t color);

    void saveRegion(
        RegionId regionId,
        int x,
        int y,
        int width,
        int height);

    void restoreRegion(
        RegionId regionId);

private:
    std::uint32_t* pixels_;

    TextRenderer textRenderer_;

    int width_;
    int height_;

    std::unordered_map<RegionId, RenderRegion> regions_;
};