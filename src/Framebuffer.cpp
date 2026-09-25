//################################################################
//Frame buffer Handler.
//Definds the handles used to Open the framebuffer, retrive information, and cleenup upon closing.
//################################################################
//Lifecycle definition:
//Constructor
//    ↓
//open()
//    ↓
//isOpen()
//    ↓
//width()
//height()
//bitsPerPixel()
//convertColor()    //This function remaps the color input to the spicific framebuffer.
//setPixel()        //sets the color of a single pixel
//fill()            //fill the entire framebuffer with a single colour
//drawRect()        //Draws a rectangle
//    ↓
//close()
//    ↓
//Destructor
//################################################################

#include "Framebuffer.hpp"

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

//The constructor initializes everything to a known "not open" state (variables defined in Framebuffer.hpp)
Framebuffer::Framebuffer()
    : fd_(-1),
      memory_(nullptr),
      memorySize_(0),
      width_(0),
      height_(0),
      bitsPerPixel_(0),
      lineLength_(0),
      redOffset_(0),
      redLength_(0),
      greenOffset_(0),
      greenLength_(0),
      blueOffset_(0),
      blueLength_(0),
      alphaOffset_(0),
      alphaLength_(0)
{
}

//the Destuctor cleans up
Framebuffer::~Framebuffer()
{
    close();
}

//Opens the framebuffer with the argument device as a C++ standart string
bool Framebuffer::open(const std::string& device)
{
    fd_ = ::open(device.c_str(), O_RDWR); //device is a C++ std::string, but Linux's open() expects a C-style string (const char*)

    if (fd_ < 0) // if it fails to open, return false
    {
        return false;
    }

    fb_var_screeninfo vinfo{};

    if (ioctl(fd_, FBIOGET_VSCREENINFO, &vinfo) < 0) //asks the framebuffer driver to fill that structure. if it fails -> close() return false
    {
        close();
        return false;
    }

    width_ = static_cast<int>(vinfo.xres);
    height_ = static_cast<int>(vinfo.yres);
    bitsPerPixel_ = static_cast<int>(vinfo.bits_per_pixel);

    redOffset_ = vinfo.red.offset;
    redLength_ = vinfo.red.length;

    greenOffset_ = vinfo.green.offset;
    greenLength_ = vinfo.green.length;

    blueOffset_ = vinfo.blue.offset;
    blueLength_ = vinfo.blue.length;

    alphaOffset_ = vinfo.transp.offset;
    alphaLength_ = vinfo.transp.length;

    fb_fix_screeninfo finfo{};

    if (ioctl(fd_, FBIOGET_FSCREENINFO, &finfo) < 0)
    {
        close();
        return false;
    }

    lineLength_ = static_cast<int>(finfo.line_length); //number of bytes occupied by one framebuffer row.

    memorySize_ = static_cast<std::size_t>(lineLength_) * vinfo.yres_virtual;

    memory_ = mmap(
        nullptr,
        memorySize_,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd_,
        0);
    
    if (memory_ == MAP_FAILED) // Check if memmory mapping succseeded.
    {
        memory_ = nullptr;
        close();
        return false;
    }

    return true;
}

//defines the function close() as part of Framebuffer class
void Framebuffer::close()
{
    if (memory_ != nullptr)
    {
        munmap(memory_, memorySize_);
        memory_ = nullptr;
    }

    if (fd_ >= 0)
    {
        ::close(fd_);
        fd_ = -1;
    }

    memorySize_ = 0;

    width_ = 0;
    height_ = 0;
    bitsPerPixel_ = 0;
    lineLength_ = 0;

    redOffset_ = 0;
    redLength_ = 0;

    greenOffset_ = 0;
    greenLength_ = 0;

    blueOffset_ = 0;
    blueLength_ = 0;

    alphaOffset_ = 0;
    alphaLength_ = 0;
}

//Query functions that provide information about the framebuffer.
bool Framebuffer::isOpen() const
{
    return fd_ >= 0 && memory_ != nullptr;
}

int Framebuffer::width() const
{
    return width_;
}

int Framebuffer::height() const
{
    return height_;
}

int Framebuffer::bitsPerPixel() const
{
    return bitsPerPixel_;
}

//Converts the application 0xAARRGGBB color into the framebuffer's
//specific color format using the channel lengths and offsets.
std::uint32_t Framebuffer::convertColor(std::uint32_t color) const
{   
    const unsigned int alpha = (color >> 24) & 0xFF; 
    const unsigned int red   = (color >> 16) & 0xFF;
    const unsigned int green = (color >> 8)  & 0xFF;
    const unsigned int blue  = color & 0xFF;

    // Converts an 8-bit color channel (0-255) into the 
    // number of bits available for that channel in the framebuffer. 
    auto scaleChannel = [](unsigned int value, unsigned int length) 
    { 
        if (length == 0) 
        { 
            return std::uint32_t{0}; 
        } 
        // The framebuffer channel has at least 8 bits. 
        // No reduction in precision is necessary. 
        if (length >= 8) 
        { 
            return static_cast<std::uint32_t>( 
                static_cast<std::uint64_t>(value) << (length - 8)); 
        } 
        // Calculate the largest value that fits in 'length' bits. 
        const std::uint32_t maxValue = (std::uint32_t{1} << length) - 1; 
        // Scale 0-255 into 0-maxValue. 
        return static_cast<std::uint32_t>( (static_cast<std::uint64_t>(value) * maxValue + 127) / 255); 
    }; 
    std::uint32_t result = 0; 

    if (redLength_ > 0) 
    { 
        result |= scaleChannel(red, redLength_) << redOffset_; 
    } 

    if (greenLength_ > 0) 
    { 
        result |= scaleChannel(green, greenLength_) << greenOffset_; 
    } 

    if (blueLength_ > 0) 
    { 
        result |= scaleChannel(blue, blueLength_) << blueOffset_; 
    } 

    if (alphaLength_ > 0) 
    { 
        result |= scaleChannel(alpha, alphaLength_) << alphaOffset_; 
    } 

    return result;
}

void Framebuffer::setPixel(int x, int y, std::uint32_t color)
{
    // Do nothing if the framebuffer is not open.
    if (!isOpen())
    {
        return;
    }

    // Do nothing if the requested pixel is outside the screen.
    if (x < 0 || x >= width_ ||
        y < 0 || y >= height_)
    {
        return;
    }

    // The current framebuffer is 32 bits per pixel,
    // so each pixel occupies 4 bytes.
    if (bitsPerPixel_ != 32)
    {
        return;
    }

    // Convert the application's 0xAARRGGBB color
    // into the framebuffer's actual pixel format.
    const std::uint32_t pixel = convertColor(color);

    // Find the beginning of the requested row.
    auto* row = static_cast<std::uint8_t*>(memory_) +
                static_cast<std::size_t>(y) * lineLength_; //Move down y rows

    // Move to the requested pixel within that row.
    auto* destination =
        reinterpret_cast<std::uint32_t*>(row) + x; //Move x 32-bit pixels across the row

    // Write the converted pixel into the framebuffer.
    *destination = pixel;
}

void Framebuffer::fill(std::uint32_t color)
{
    // Do nothing if the framebuffer is not open.
    if (!isOpen())
    {
        return;
    }

    // The current framebuffer is 32 bits per pixel.
    if (bitsPerPixel_ != 32)
    {
        return;
    }

    // Convert the application's 0xAARRGGBB color
    // into the framebuffer's actual pixel format.
    // All pixels are identicle. So there is only need to doo this once.
    const std::uint32_t pixel = convertColor(color);

    // Go through every row of the framebuffer.
    for (int y = 0; y < height_; ++y)
    {
        // Find the beginning of the current row.
        auto* row = static_cast<std::uint8_t*>(memory_) +
                    static_cast<std::size_t>(y) * lineLength_;

        // Go through every pixel in the current row.
        for (int x = 0; x < width_; ++x)
        {
            auto* destination =
                reinterpret_cast<std::uint32_t*>(row) + x;

            *destination = pixel;
        }
    }
}

void Framebuffer::drawRect(
    int x,
    int y,
    int width,
    int height,
    std::uint32_t color)
{
    // Do nothing if the framebuffer is not open.
    if (!isOpen())
    {
        return;
    }

    // Do nothing if the rectangle has no size.
    if (width <= 0 || height <= 0)
    {
        return;
    }

    // Draw each row of the rectangle.
    for (int currentY = y;
         currentY < y + height;
         ++currentY)
    {
        // Draw each pixel in the current row.
        for (int currentX = x;
             currentX < x + width;
             ++currentX)
        {
            setPixel(currentX, currentY, color);
        }
    }
}