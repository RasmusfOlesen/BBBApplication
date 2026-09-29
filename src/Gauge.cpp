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
    renderer_.drawLine(
        centerX_ - 67,
        centerY_ - 17,
        centerX_ - 79,
        centerY_ - 29,
        6,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ - 41,
        centerY_ - 35,
        centerX_ - 49,
        centerY_ - 50,
        6,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ + 1,
        centerY_ - 44,
        centerX_ + 1,
        centerY_ - 61,
        6,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ + 47,
        centerY_ - 31,
        centerX_ + 55,
        centerY_ - 46,
        6,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ + 63,
        centerY_ - 19,
        centerX_ + 75,
        centerY_ - 31,
        6,
        0x00FFFFFF,
        LineCap::Butt);

    //Draw Minor Tick marks
        renderer_.drawLine(
        centerX_ - 54,
        centerY_ - 27,
        centerX_ - 64,
        centerY_ - 41,
        3,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ - 20,
        centerY_ - 42,
        centerX_ - 24,
        centerY_ - 58,
        3,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ + 25,
        centerY_ - 41,
        centerX_ + 29,
        centerY_ - 57,
        3,
        0x00FFFFFF,
        LineCap::Butt);

    renderer_.drawLine(
        centerX_ + 57,
        centerY_ - 24,
        centerX_ + 68,
        centerY_ - 38,
        3,
        0x00FFFFFF,
        LineCap::Butt);

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

