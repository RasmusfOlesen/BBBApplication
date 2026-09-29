#include "Gauge.hpp"

Gauge::Gauge(
    Renderer& renderer,
    int centerX,
    int centerY)
     :renderer_(renderer),
      needle_(),
      centerX_(centerX),
      centerY_(centerY),
      needleX_(centerX),
      needleY_(centerY + 51)
{
}

uint32_t ScaleColor = 0x00FFFFFF;

void Gauge::drawBackground()
{
/*    
    // Guidelines

    //Outside scale Arc
    renderer_.drawArc(
        needleX_,
        needleY_,
        112,
        45,
        135,
        1,
        0x00808080);

    //Inside Scale arc
    renderer_.drawArc(
        needleX_,
        needleY_,
        94,
        45,
        135,
        1,
        0x00808080);

    //Needle boundery box
    renderer_.drawLine(
        needleX_ - 102,
        needleY_ - 102,
        needleX_ + 102,
        needleY_ - 102,
        1,
        0x00808080,
        LineCap::Round);

    renderer_.drawLine(
        needleX_ + 102,
        needleY_ - 102,
        needleX_ + 102,
        needleY_ + 102,
        1,
        0x00808080,
        LineCap::Round);

    renderer_.drawLine(
        needleX_ + 102,
        needleY_ + 102,
        needleX_ - 102,
        needleY_ + 102,
        1,
        0x00808080,
        LineCap::Round);

    renderer_.drawLine(
        needleX_ - 102,
        needleY_ + 102,
        needleX_ - 102,
        needleY_ - 102,
        1,
        0x00808080,
        LineCap::Round);
*/
    renderer_.drawCircle(
        centerX_,
        centerY_,
        127,
        3,
        0x00C0C0C0);

    //Draw Major Tick marks
    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        45.0,
        6,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        28.0,
        6,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        0.0,
        6,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        -28.0,
        6,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        -45.0,
        6,
        0x00FF0000);

    //Draw Minor Tick marks
    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        36.0,
        3,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        14.0,
        3,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        -14.0,
        3,
        0x00FFFFFF);

    renderer_.drawRotatedLine(
        0,
        -112,
        0,
        -95,
        needleX_,
        needleY_,
        -39.0,
        3,
        0x00FF0000);

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
        "100",
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

void Gauge::setNeedleAngle(double angle)
{
    needle_.setAngle(angle);
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

