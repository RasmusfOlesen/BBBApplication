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
      height_(height),
      textRenderer_(pixels, width, height)
{
}

void Renderer::setTarget(std::uint32_t* pixels)
{
    pixels_ = pixels;

    textRenderer_.setTarget(
        pixels);
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

void Renderer::drawPolygon(
    const std::vector<Point>& points,
    std::uint32_t color)
{
    if (points.size() < 3)
    {
        return;
    }

    int minY = points[0].y;
    int maxY = points[0].y;

    for (const Point& point : points)
    {
        minY = std::min(minY, point.y);
        maxY = std::max(maxY, point.y);
    }

    for (int y = minY; y <= maxY; ++y)
    {
        std::vector<int> intersections;

        for (std::size_t i = 0; i < points.size(); ++i)
        {
            const Point& p1 = points[i];
            const Point& p2 =
                points[(i + 1) % points.size()];

            if (p1.y == p2.y)
            {
                continue;
            }

            if (y < std::min(p1.y, p2.y) ||
                y >= std::max(p1.y, p2.y))
            {
                continue;
            }

            double x =
                p1.x +
                static_cast<double>(y - p1.y) *
                static_cast<double>(p2.x - p1.x) /
                static_cast<double>(p2.y - p1.y);

            intersections.push_back(
                static_cast<int>(std::round(x)));
        }

        std::sort(
            intersections.begin(),
            intersections.end());

        for (std::size_t i = 0;
             i + 1 < intersections.size();
             i += 2)
        {
            for (int x = intersections[i];
                 x <= intersections[i + 1];
                 ++x)
            {
                setPixel(x, y, color);
            }
        }
    }
}

void Renderer::drawLine(
    int x0,
    int y0,
    int x1,
    int y1,
    int thickness,
    std::uint32_t color,
    LineCap cap)
{
    if (thickness <= 0) // Reject invalid thickness values.
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

    // A thick line is treated as a filled stroke. with LineCap round
    // Pixels are accepted if their distance to the
    // line segment is less than the stroke radius.
    //------------------------------------------------------
    // Calculate line direction and length.
    double dx =
        static_cast<double>(x1 - x0);

    double dy =
        static_cast<double>(y1 - y0);

    double length =
        std::sqrt(dx * dx + dy * dy);

    // Stroke radius.
    double radius =
        static_cast<double>(thickness) / 2.0;

    //LineCap Square
    // Extend the endpoints by half the stroke width
    // in the line direction.
    if (length > 0.0 &&
        cap == LineCap::Square)
    {
        double extendX =
            dx / length * radius;

        double extendY =
            dy / length * radius;

        x0 -= static_cast<int>(std::round(extendX));
        y0 -= static_cast<int>(std::round(extendY));

        x1 += static_cast<int>(std::round(extendX));
        y1 += static_cast<int>(std::round(extendY));

        dx =
            static_cast<double>(x1 - x0);

        dy =
            static_cast<double>(y1 - y0);
    }
    //------------------------------------------------------
    // Precalculate line length squared.
    // Used repeatedly when projecting pixels onto
    // the line segment.
    double lengthSquared =
        dx * dx + dy * dy;
     
    // Calculate a bounding box around the stroke.
    // Only pixels inside this area need testing.
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

    //------------------------------------------------------
    // Scan every pixel in the bounding box.
    //------------------------------------------------------
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            // Convert pixel coordinates to doubles.
            double px =
                static_cast<double>(x);

            double py =
                static_cast<double>(y);

            // Project the pixel onto the line.
            // t:
            // 0.0 = start point
            // 1.0 = end point
            // Values outside [0,1] lie beyond the segment.
            double t = 0.0;

            if (lengthSquared > 0.0)
            {
                t =
                    ((px - x0) * dx +
                    (py - y0) * dy)
                    / lengthSquared;

                // Butt caps. Reject pixels beyond the endpoints.
                if (cap == LineCap::Butt &&
                    (t < 0.0 || t > 1.0))
                {
                    continue;
                }

                // Round and square caps. Clamp projection to the nearest endpoint.
                if (t < 0.0)
                {
                    t = 0.0;
                }
                else if (t > 1.0)
                {
                    t = 1.0;
                }
            }

            // Find the closest point on the line.
            double closestX =
                x0 + t * dx;

            double closestY =
                y0 + t * dy;

            // Distance from pixel to line.
            double distanceX =
                px - closestX;

            double distanceY =
                py - closestY;

            double distanceSquared =
                distanceX * distanceX +
                distanceY * distanceY;

            // Draw pixel if it lies inside the stroke radius.
            if (distanceSquared <= radius * radius)
            {
                setPixel(x, y, color);
            }
        }
    }
}

void Renderer::drawRotatedLine(
    int startX,
    int startY,
    int endX,
    int endY,
    int originX,
    int originY,
    double angle,
    int thickness,
    std::uint32_t color,
    LineCap cap)
{
    double angleRadians =
        angle * M_PI / 180.0;

    double cosAngle =
        std::cos(angleRadians);

    double sinAngle =
        std::sin(angleRadians);

    int rotatedStartX =
        originX +
        static_cast<int>(
            std::round(
                startX * cosAngle +
                startY * sinAngle));

    int rotatedStartY =
        originY +
        static_cast<int>(
            std::round(
                -startX * sinAngle +
                startY * cosAngle));

    int rotatedEndX =
        originX +
        static_cast<int>(
            std::round(
                endX * cosAngle +
                endY * sinAngle));

    int rotatedEndY =
        originY +
        static_cast<int>(
            std::round(
                -endX * sinAngle +
                endY * cosAngle));

    drawLine(
        rotatedStartX,
        rotatedStartY,
        rotatedEndX,
        rotatedEndY,
        thickness,
        color,
        cap);
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
    std::uint32_t color,
    LineCap cap)
{
    // Reject invalid radius or thickness.
    if (radius <= 0 || thickness <= 0) 
    {
        return;
    }

    //------------------------------------------------------
    // Calculate the inner and outer radius of the stroke.
    //
    // The requested radius represents the center of the
    // stroke, while thickness extends on both sides.
    //------------------------------------------------------
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

    //------------------------------------------------------
    // Square caps extend the arc beyond its requested
    // start and end angles.
    //
    // The angular extension is based on half the stroke
    // width relative to the arc radius.
    //------------------------------------------------------
    double effectiveStartAngle = startAngle;
    double effectiveEndAngle = endAngle;

    if (cap == LineCap::Square)
    {
        double extensionAngle =
            (static_cast<double>(thickness) / 2.0) /
            static_cast<double>(radius) *
            180.0 / M_PI;

        effectiveStartAngle -= extensionAngle;
        effectiveEndAngle += extensionAngle;
    }

    //------------------------------------------------------
    // Calculate the bounding box containing the entire
    // circular stroke.
    //------------------------------------------------------
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

    //------------------------------------------------------
    // Scan every pixel inside the bounding box.
    //------------------------------------------------------
    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            //------------------------------------------------------
            // Calculate pixel position relative to the
            // center of the arc.
            //------------------------------------------------------
            double dx =
                static_cast<double>(x - centerX);

            double dy =
                static_cast<double>(y - centerY);

            //------------------------------------------------------
            // Calculate distance from the center.
            //------------------------------------------------------
            double distance =
                std::sqrt(dx * dx + dy * dy);

            //------------------------------------------------------
            // Reject pixels outside the stroke thickness.
            //------------------------------------------------------
            if (distance < innerRadius ||
                distance > outerRadius)
            {
                continue;
            }

            //------------------------------------------------------
            // Calculate the pixel angle.
            //
            // -dy gives us the mathematical angle convention:
            //
            //     0°   = right
            //     90°  = up
            //     180° = left
            //     270° = down
            //------------------------------------------------------
            double angle =
                std::atan2(-dy, dx)
                * 180.0 / M_PI;

            if (angle < 0.0)
            {
                angle += 360.0;
            }

            //------------------------------------------------------
            // Determine whether the pixel angle is inside
            // the requested arc.
            //
            // The second case handles arcs crossing 0°.
            //------------------------------------------------------
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

            //------------------------------------------------------
            // Draw the pixel if it belongs to the arc.
            //------------------------------------------------------
            if (angleInRange)
            {
                setPixel(
                    x,
                    y,
                    color);
            }
        }
    }

    //------------------------------------------------------
    // Round caps.
    //
    // Add a circle with the stroke radius at each endpoint.
    //
    // The circle radius is half the line thickness.
    //------------------------------------------------------
    if (cap == LineCap::Round)
    {
        double startRadians =
            startAngle * M_PI / 180.0;

        double endRadians =
            endAngle * M_PI / 180.0;

        int startX =
            static_cast<int>(
                std::round(
                    centerX +
                    std::cos(startRadians) * radius));

        int startY =
            static_cast<int>(
                std::round(
                    centerY -
                    std::sin(startRadians) * radius));

        int endX =
            static_cast<int>(
                std::round(
                    centerX +
                    std::cos(endRadians) * radius));

        int endY =
            static_cast<int>(
                std::round(
                    centerY -
                    std::sin(endRadians) * radius));

        fillCircle(
            startX,
            startY,
            thickness / 2,
            color);

        fillCircle(
            endX,
            endY,
            thickness / 2,
            color);
    }
}

void Renderer::drawText(
    const char* text,
    int x,
    int y,
    int size,
    std::uint32_t color)
{
    textRenderer_.drawText(
        text,
        x,
        y,
        size,
        color);
}

void Renderer::saveRegion(
    RegionId regionId,
    int x,
    int y,
    int width,
    int height)
{
    RenderRegion& region = regions_[regionId];

    region.x = x;
    region.y = y;
    region.width = width;
    region.height = height;

    region.pixels.resize(
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height));

    for (int yOffset = 0; yOffset < height; ++yOffset)
    {
        for (int xOffset = 0; xOffset < width; ++xOffset)
        {
            region.pixels[
                yOffset * width + xOffset] =
                pixels_[
                    (y + yOffset) * width_ +
                    (x + xOffset)];
        }
    }
}

void Renderer::restoreRegion(
    RegionId regionId)
{
    RenderRegion& region = regions_[regionId];

    for (int yOffset = 0; yOffset < region.height; ++yOffset)
    {
        for (int xOffset = 0; xOffset < region.width; ++xOffset)
        {
            pixels_[
                (region.y + yOffset) * width_ +
                (region.x + xOffset)] =
                region.pixels[
                    yOffset * region.width +
                    xOffset];
        }
    }
}

