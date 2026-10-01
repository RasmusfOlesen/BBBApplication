#pragma once

#include "Renderer.hpp"
#include "Needle.hpp"
#include "GaugeScale.hpp"

class Gauge
{
public:
    Gauge(
        Renderer& renderer,
        int centerX,
        int centerY);

    void drawBackground();
    void drawNeedle();
    void drawValue();

    void setValue(double value);
    
    RenderRegion needleRegion() const;

private:
    Renderer& renderer_;

    Needle needle_;

    GaugeScale scale_;

    int centerX_;
    int centerY_;

    int needleX_;
    int needleY_;

    double value_;
};