#include "Renderer.hpp"

#include <cstdlib>
#include <cmath>
#include <algorithm>

//Constructor
Renderer::Renderer(
    std::uint32_t* pixels,
    int width,
    int height)
    : pixels_(pixels),
      width_(width),
      height_(height)
{
}

void Renderer::setTarget(std::uint32_t* pixels)
{
    pixels_ = pixels;
}

void Renderer::setPixel(int x,int y,std::uint32_t color)
{
    if (x < 0 || x >= width_ ||
        y < 0 || y >= height_)
    {
        return;
    }

    pixels_[y * width_ + x] = color;
}

void Renderer::fill(std::uint32_t color)
{
    for (int y = 0; y < height_; ++y)
    {
        for (int x = 0; x < width_; ++x)
        {
            pixels_[y * width_ + x] = color;
        }
    }
}

void Renderer::drawRect(int x,int y,int width,int height,std::uint32_t color)
{
    for (int currentY = y;
         currentY < y + height;
         ++currentY)
    {
        for (int currentX = x;
             currentX < x + width;
             ++currentX)
        {
            setPixel(
                currentX,
                currentY,
                color);
        }
    }
}

void Renderer::drawLine(
    int x0,
    int y0,
    int x1,
    int y1,
    int thickness,
    std::uint32_t color)
{
    if (thickness <= 0)
    {
        return;
    }

    // A one-pixel line uses Bresenham's algorithm.
    if (thickness == 1)
    {
        int dx = std::abs(x1 - x0);
        int sx = x0 < x1 ? 1 : -1;

        int dy = -std::abs(y1 - y0);
        int sy = y0 < y1 ? 1 : -1;

        int error = dx + dy;

        while (true)
        {
            setPixel(x0, y0, color);

            if (x0 == x1 && y0 == y1)
            {
                break;
            }

            int error2 = 2 * error;

            if (error2 >= dy)
            {
                error += dy;
                x0 += sx;
            }

            if (error2 <= dx)
            {
                error += dx;
                y0 += sy;
            }
        }

        return;
    }

    // A thick line is treated as a filled stroke.
    double dx =
        static_cast<double>(x1 - x0);

    double dy =
        static_cast<double>(y1 - y0);

    double lengthSquared =
        dx * dx + dy * dy;

    double radius =
        static_cast<double>(thickness) / 2.0;

    int minX =
        static_cast<int>(
            std::floor(
                std::min(x0, x1) - radius));

    int maxX =
        static_cast<int>(
            std::ceil(
                std::max(x0, x1) + radius));

    int minY =
        static_cast<int>(
            std::floor(
                std::min(y0, y1) - radius));

    int maxY =
        static_cast<int>(
            std::ceil(
                std::max(y0, y1) + radius));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            double px =
                static_cast<double>(x);

            double py =
                static_cast<double>(y);

            double t = 0.0;

            if (lengthSquared > 0.0)
            {
                t =
                    ((px - x0) * dx +
                     (py - y0) * dy)
                    / lengthSquared;

                if (t < 0.0)
                {
                    t = 0.0;
                }
                else if (t > 1.0)
                {
                    t = 1.0;
                }
            }

            double closestX =
                x0 + t * dx;

            double closestY =
                y0 + t * dy;

            double distanceX =
                px - closestX;

            double distanceY =
                py - closestY;

            double distanceSquared =
                distanceX * distanceX +
                distanceY * distanceY;

            if (distanceSquared <= radius * radius)
            {
                setPixel(x, y, color);
            }
        }
    }
}

void Renderer::drawCircle(
    int centerX,
    int centerY,
    int radius,
    int thickness,
    std::uint32_t color)
{
    if (radius <= 0 || thickness <= 0)
    {
        return;
    }

    double outerRadius =
        static_cast<double>(radius) +
        static_cast<double>(thickness) / 2.0;

    double innerRadius =
        static_cast<double>(radius) -
        static_cast<double>(thickness) / 2.0;

    if (innerRadius < 0.0)
    {
        innerRadius = 0.0;
    }

    int minX =
        static_cast<int>(
            std::floor(centerX - outerRadius));

    int maxX =
        static_cast<int>(
            std::ceil(centerX + outerRadius));

    int minY =
        static_cast<int>(
            std::floor(centerY - outerRadius));

    int maxY =
        static_cast<int>(
            std::ceil(centerY + outerRadius));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            double dx =
                static_cast<double>(x - centerX);

            double dy =
                static_cast<double>(y - centerY);

            double distance =
                std::sqrt(dx * dx + dy * dy);

            if (distance >= innerRadius &&
                distance <= outerRadius)
            {
                setPixel(x, y, color);
            }
        }
    }
}

void Renderer::fillCircle(
    int centerX,
    int centerY,
    int radius,
    std::uint32_t color)
{
    if (radius <= 0)
    {
        return;
    }

    int radiusSquared = radius * radius;

    for (int y = -radius; y <= radius; ++y)
    {
        for (int x = -radius; x <= radius; ++x)
        {
            if ((x * x + y * y) <= radiusSquared)
            {
                setPixel(
                    centerX + x,
                    centerY + y,
                    color);
            }
        }
    }
}

void Renderer::drawArc(
    int centerX,
    int centerY,
    int radius,
    double startAngle,
    double endAngle,
    int thickness,
    std::uint32_t color)
{
    if (radius <= 0 || thickness <= 0)
    {
        return;
    }

    double outerRadius =
        static_cast<double>(radius) +
        static_cast<double>(thickness) / 2.0;

    double innerRadius =
        static_cast<double>(radius) -
        static_cast<double>(thickness) / 2.0;

    if (innerRadius < 0.0)
    {
        innerRadius = 0.0;
    }

    int minX =
        static_cast<int>(
            std::floor(centerX - outerRadius));

    int maxX =
        static_cast<int>(
            std::ceil(centerX + outerRadius));

    int minY =
        static_cast<int>(
            std::floor(centerY - outerRadius));

    int maxY =
        static_cast<int>(
            std::ceil(centerY + outerRadius));

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            double dx =
                static_cast<double>(x - centerX);

            double dy =
                static_cast<double>(y - centerY);

            double distance =
                std::sqrt(dx * dx + dy * dy);

            if (distance < innerRadius ||
                distance > outerRadius)
            {
                continue;
            }

            double angle =
                std::atan2(-dy, dx) //-dy sets the angle convension 0-right 90-up
                * 180.0 / M_PI;

            if (angle < 0.0)
            {
                angle += 360.0;
            }

            bool angleInRange;

            if (startAngle <= endAngle)
            {
                angleInRange =
                    angle >= startAngle &&
                    angle <= endAngle;
            }
            else
            {
                angleInRange =
                    angle >= startAngle ||
                    angle <= endAngle;
            }

            if (angleInRange)
            {
                setPixel(
                    x,
                    y,
                    color);
            }
        }
    }
}