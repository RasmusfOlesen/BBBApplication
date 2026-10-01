#include "Gauge.hpp"

#include <string>
#include <sstream>

Gauge::Gauge(
    Renderer& renderer,
    int centerX,
    int centerY)
     :renderer_(renderer),
      needle_(),
      centerX_(centerX),
      centerY_(centerY),
      needleX_(centerX),
      needleY_(centerY + 51),
      value_(0.0)  
{
    scale_.setMapping({
        { 40.0, 145.0 },
        { 50.0, 135.0 },
        {120.0,  90.0 },
        {150.0,  45.0 }
    });

    scale_.setTicks({
        { 50.0, TickType::Major, 0x00FFFFFF },

        { 60.0, TickType::Minor, 0x00808080 },
        { 70.0, TickType::Minor, 0x00808080 },
        { 80.0, TickType::Minor, 0x00808080 },
        { 90.0, TickType::Minor, 0x00808080 },
        {100.0, TickType::Minor, 0x00808080 },
        {110.0, TickType::Minor, 0x00808080 },

        {120.0, TickType::Major, 0x00FFFFFF },

        {130.0, TickType::Minor, 0x00808080 },
        {140.0, TickType::Minor, 0x00808080 },

        {150.0, TickType::Major, 0x00FFFFFF }
    });
}

uint32_t ScaleColor = 0x00FFFFFF;

void Gauge::drawBackground()
{
    //Draw Tickmarks based on scale definition
    for (const ScaleTick& tick : scale_.ticks())
    {
        double angle =
            scale_.valueToAngle(tick.value);

        int startX;
        int startY;
        int endX;
        int endY;
        int thickness;

        if (tick.type == TickType::Major)
        {
            startX = 112;
            startY = 0;

            endX = 95;
            endY = 0;

            thickness = 6;
        }
        else if (tick.type == TickType::Minor)
        {
            startX = 112;
            startY = 0;

            endX = 95;
            endY = 0;

            thickness = 3;
        }
        else
        {
            continue;
        }

        renderer_.drawRotatedLine(
            startX,
            startY,
            endX,
            endY,
            needleX_,
            needleY_,
            angle,
            thickness,
            tick.color);
    }


    // Draw gauge outline
    renderer_.drawCircle(
        centerX_,
        centerY_,
        127,
        3,
        0x00C0C0C0);

   
    renderer_.drawText(
        "Auto °C",
        centerX_ - 30,
        centerY_ - 70,
        30,
        0x00FFFFFF);

    renderer_.drawText(
        "50",
        centerX_ - 67,
        centerY_ + 3,
        20,
        0x00FFFFFF);

    renderer_.drawText(
        "120",
        centerX_ - 13,
        centerY_ - 25,
        20,
        0x00FFFFFF);

    renderer_.drawText(
        "150",
        centerX_ + 37,
        centerY_ + 3,
        20,
        0x00FFFFFF);
}

void Gauge::drawNeedle()
{
    //Draw Needle
        needle_.draw(
        renderer_,
        needleX_,
        needleY_);

    // Needle hub
    renderer_.fillCircle(
        needleX_,
        needleY_,
        22,
        0x00202020);
}

void Gauge::drawValue()
{
    std::ostringstream ss;
    ss << value_;

    std::string valueText = ss.str();

    renderer_.drawRect(
        needleX_ - 50,
        needleY_,
        100,
        50,
        0x00000000);

    renderer_.drawText(
        valueText.c_str(),
        needleX_ - 25,
        needleY_ + 25,
        20,
        0x00FFFFFF);
}

void Gauge::setValue(double value)
{
    value_ = value;

    needle_.setAngle(
        scale_.valueToAngle(value));
}

RenderRegion Gauge::needleRegion() const
{
    return {
        needleX_ - 102,
        needleY_ - 102,
        204,
        204,
        {}
    };
}

