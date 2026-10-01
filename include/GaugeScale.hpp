#pragma once

#include <cstdint>
#include <vector>

struct ScalePoint
{
    double value;
    double angle;
};

enum class TickType
{
    None,
    Minor,
    Major
};

struct ScaleTick
{
    double value;
    TickType type;
    std::uint32_t color;
};

class GaugeScale
{
public:
    void setMapping(
        const std::vector<ScalePoint>& mapping);

    void setTicks(
        const std::vector<ScaleTick>& ticks);

    double valueToAngle(
        double value) const;

    const std::vector<ScaleTick>& ticks() const;

private:
    std::vector<ScalePoint> mapping_;
    std::vector<ScaleTick> ticks_;
};