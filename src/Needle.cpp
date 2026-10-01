#include "Needle.hpp"
#include "Renderer.hpp"

#include <cmath>

Needle::Needle() //Needle right with respect to origin
    : darkPoints_
    //Base of needle
    {
        {101, 0}, //Tip
		{96, 2},
		{19, 6},  //Base to the right
		{19, -6}, //Base to the left
		{96, -2}
    },

    lightPoints_
    //light colord region on top of dark
    {
        {101, 0}, //Tip
		{19, 0},  //Base to the right
		{19, -6}, //Base to the left
		{96, -2}
    },

    angle_(0.0),
    colorLight_(0x00FF0000),
     colorDark_(0x00B00000)
{
}

void Needle::setAngle(double angle)
{
    angle_ = angle;
}

std::vector<Point> Needle::rotatePoints(
    const std::vector<Point>& points,
    int pivotX,
    int pivotY) const
{
    std::vector<Point> rotated;
    rotated.reserve(points.size());

    double angleRadians =
        angle_ * M_PI / 180.0;

    double cosAngle =
        std::cos(angleRadians);

    double sinAngle =
        std::sin(angleRadians);

    for (const Point& point : points)
    {
        double rotatedX =
            point.x * cosAngle +
            point.y * sinAngle;

        double rotatedY =
          - point.x * sinAngle +
            point.y * cosAngle;

        rotated.push_back(
            {
                pivotX + static_cast<int>(std::round(rotatedX)),
                pivotY + static_cast<int>(std::round(rotatedY))
            });
    }

    return rotated;
}

void Needle::draw(
    Renderer& renderer,
    int pivotX,
    int pivotY) const
{
    std::vector<Point> rotatedDark =
        rotatePoints(
            darkPoints_,
            pivotX,
            pivotY);

    std::vector<Point> rotatedLight =
        rotatePoints(
            lightPoints_,
            pivotX,
            pivotY);

    renderer.drawPolygon(
        rotatedDark,
        colorDark_);

    renderer.drawPolygon(
        rotatedLight,
        colorLight_);
}

