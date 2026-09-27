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

    // Open the DRM device used to control the display.
    bool open(const std::string& device = "/dev/dri/card0");
    void close();

    bool present(bool newFrameAvailable);

    bool isOpen() const;

    int width() const;
    int height() const;

private:
    int fd_;

    uint32_t crtcId_;

    uint32_t originalFramebufferId_;

    int activeBufferIndex_;
    bool pageFlipPending_;
    
    //create a struct to hold all the variable to handle creation of a framebuffer
    struct Buffer
    {
        uint32_t framebufferId = 0;
        uint32_t dumbBufferHandle = 0;
        void* memory = nullptr;
        std::size_t size = 0;
    };
    //create two instances for the amougnt of framebuffers needed.
    Buffer buffers_[2];

    int width_;
    int height_;
    
    static void pageFlipHandler(
        int fd,
        unsigned int frame,
        unsigned int sec,
        unsigned int usec,
        void* data);
};