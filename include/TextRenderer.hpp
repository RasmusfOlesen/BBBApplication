#pragma once

#include <cstdint>
#include <ft2build.h>
#include FT_FREETYPE_H

class TextRenderer
{
public:
    TextRenderer(
        std::uint32_t* pixels,
        int width,
        int height);

    void setTarget(
        std::uint32_t* pixels);

    void drawCharacter(
        std::uint32_t character,
        int x,
        int y,
        int size,
        std::uint32_t color);

    void drawText(
        const char* text,
        int x,
        int y,
        int size,
        std::uint32_t color);

private:
    FT_Library library_;
    FT_Face face_;

    std::uint32_t* pixels_;

    int width_;
    int height_;
};