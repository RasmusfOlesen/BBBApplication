#include "TextRenderer.hpp"

TextRenderer::TextRenderer(
    std::uint32_t* pixels,
    int width,
    int height)
{
    pixels_ = pixels;
    width_ = width;
    height_ = height;

    FT_Init_FreeType(&library_);

    FT_New_Face(
        library_,
        "/home/Beagle/fonts/din1451alt.ttf",
        0,
        &face_);
}

void TextRenderer::setTarget(
    std::uint32_t* pixels)
{
    pixels_ = pixels;
}

void TextRenderer::drawCharacter(
    std::uint32_t character,
    int x,
    int y,
    int size,
    std::uint32_t color)
{
    FT_Set_Pixel_Sizes(
        face_,
        0,
        size);

    FT_Load_Char(
        face_,
        character,
        FT_LOAD_RENDER);

    FT_Bitmap& bitmap = face_->glyph->bitmap;

    for (unsigned int row = 0; row < bitmap.rows; ++row)
    {
        for (unsigned int column = 0; column < bitmap.width; ++column)
        {
            unsigned char alpha =
                bitmap.buffer[
                    row * bitmap.pitch + column];

            if (alpha == 0)
            {
                continue;
            }

            int pixelX =
                x +
                face_->glyph->bitmap_left +
                static_cast<int>(column);

            int pixelY =
                y -
                face_->glyph->bitmap_top +
                static_cast<int>(row);

            if (pixelX < 0 ||
                pixelX >= width_ ||
                pixelY < 0 ||
                pixelY >= height_)
            {
                continue;
            }

            pixels_[
                pixelY * width_ + pixelX] =
                color;
        }
    }
}

void TextRenderer::drawText(
    const char* text,
    int x,
    int y,
    int size,
    std::uint32_t color)
{
    int cursorX = x;

    while (*text != '\0')
    {
        std::uint32_t codePoint;

        unsigned char first =
            static_cast<unsigned char>(*text);

        if (first < 0x80)
        {
            codePoint = first;
            text += 1;
        }
        else if ((first & 0xE0) == 0xC0)
        {
            codePoint =
                (static_cast<std::uint32_t>(first & 0x1F) << 6) |
                (static_cast<std::uint32_t>(
                    static_cast<unsigned char>(text[1]) & 0x3F));

            text += 2;
        }
        else if ((first & 0xF0) == 0xE0)
        {
            codePoint =
                (static_cast<std::uint32_t>(first & 0x0F) << 12) |
                (static_cast<std::uint32_t>(
                    static_cast<unsigned char>(text[1]) & 0x3F) << 6) |
                static_cast<std::uint32_t>(
                    static_cast<unsigned char>(text[2]) & 0x3F);

            text += 3;
        }
        else
        {
            codePoint =
                (static_cast<std::uint32_t>(first & 0x07) << 18) |
                (static_cast<std::uint32_t>(
                    static_cast<unsigned char>(text[1]) & 0x3F) << 12) |
                (static_cast<std::uint32_t>(
                    static_cast<unsigned char>(text[2]) & 0x3F) << 6) |
                static_cast<std::uint32_t>(
                    static_cast<unsigned char>(text[3]) & 0x3F);

            text += 4;
        }

        drawCharacter(
            codePoint,
            cursorX,
            y,
            size,
            color);

        FT_Load_Char(
            face_,
            codePoint,
            FT_LOAD_DEFAULT);

        cursorX +=
            static_cast<int>(
                face_->glyph->advance.x >> 6);
    }
}

