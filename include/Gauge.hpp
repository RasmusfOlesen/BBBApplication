#pragma once

#include "Renderer.hpp"
#include "Needle.hpp"

class Gauge
{
public:
    Gauge(
        Renderer& renderer,
        int centerX,
        int centerY);

    void drawBackground();
    void drawNeedle();

    void setNeedleAngle(double angle);
    
    RenderRegion needleRegion() const;

private:
    Renderer& renderer_;

    Needle needle_;

    int centerX_;
    int centerY_;

    int needleX_;
    int needleY_;
};