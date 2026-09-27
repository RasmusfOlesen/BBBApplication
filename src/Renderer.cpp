#include "Renderer.hpp"

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