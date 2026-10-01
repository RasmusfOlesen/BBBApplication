#include "GaugeScale.hpp"

void GaugeScale::setMapping(
    const std::vector<ScalePoint>& mapping)
{
    mapping_ = mapping;
}

void GaugeScale::setTicks(
    const std::vector<ScaleTick>& ticks)
{
    ticks_ = ticks;
}

double GaugeScale::valueToAngle(
    double value) const
{
    if (value <= mapping_.front().value)
    {
        return mapping_.front().angle;
    }

    if (value >= mapping_.back().value)
    {
        return mapping_.back().angle;
    }

    for (std::size_t i = 1; i < mapping_.size(); ++i)
    {
        const ScalePoint& previous =
            mapping_[i - 1];

        const ScalePoint& next =
            mapping_[i];

        if (value <= next.value)
        {
            double fraction =
                (value - previous.value) /
                (next.value - previous.value);

            return previous.angle +
                   fraction *
                   (next.angle - previous.angle);
        }
    }

    return mapping_.back().angle;
}

const std::vector<ScaleTick>& GaugeScale::ticks() const
{
    return ticks_;
}