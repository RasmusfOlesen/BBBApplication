#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

class Framebuffer
{
public:
    Framebuffer();
    ~Framebuffer();

    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;

    bool open(const std::string& device = "/dev/fb0");
    void close();

    bool isOpen() const;

    int width() const;
    int height() const;
    int virtualWidth() const;
    int bitsPerPixel() const;
    
    bool waitForVSync();

    void setPixel(int x, int y, std::uint32_t color);
    void fill(std::uint32_t color);

    void drawRect(
        int x,
        int y,
        int width,
        int height,
        std::uint32_t color);

    bool saveRegion(
        int x,
        int y,
        int width,
        int height,
        std::vector<std::uint32_t>& buffer);

    bool restoreRegion(
        int x,
        int y,
        int width,
        int height,
        const std::vector<std::uint32_t>& buffer);
    
    void drawBuffer(
        const std::vector<std::uint32_t>& buffer);

private:
    int fd_;
    void* memory_;
    std::size_t memorySize_;

    int width_;
    int height_;
    int virtualWidth_;
    int bitsPerPixel_;
    int lineLength_;

    unsigned int redOffset_;
    unsigned int redLength_;

    unsigned int greenOffset_;
    unsigned int greenLength_;

    unsigned int blueOffset_;
    unsigned int blueLength_;

    unsigned int alphaOffset_;
    unsigned int alphaLength_;

    std::uint32_t convertColor(std::uint32_t color) const;
};