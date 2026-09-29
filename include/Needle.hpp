#pragma once
#include "Renderer.hpp"

#include <vector>
#include <cstdint>

class Needle
{
public:
    Needle();

    void setAngle(double angle);

    void draw(
        Renderer& renderer,
        int pivotX,
        int pivotY) const;

private:
    double angle_;

    std::vector<Point> lightPoints_;
    std::vector<Point> darkPoints_;

    std::uint32_t colorLight_;
    std::uint32_t colorDark_;

    std::vector<Point> rotatePoints(
        const std::vector<Point>& points,
        int pivotX,
        int pivotY) const;
};