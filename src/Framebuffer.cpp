//################################################################
// Framebuffer / DRM display handler.
//
// Handles DRM device ownership, framebuffer creation,
// page flipping, and restoration of the original display state.
//################################################################
//Known Bugs:
//
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
//    ↓
//close()
//    ↓
//Destructor
//################################################################

#include "Framebuffer.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm.h>
#include <drm_fourcc.h>
#include <poll.h>
#include <iostream> //for the diagnosics block

//The constructor initializes everything to a known "not open" state (variables defined in Framebuffer.hpp)
Framebuffer::Framebuffer()
    : fd_(-1),
      crtcId_(0),
      originalFramebufferId_(0),
      activeBufferIndex_(0),
      pageFlipPending_(false),
      width_(0),
      height_(0)
{
}

//the Destuctor cleans up
Framebuffer::~Framebuffer()
{
    close();
}

//Opens the framebuffer with the argument device as a C++ standart string
bool Framebuffer::open(const std::string& device)
//Document what Open does.
{
    fd_ = ::open(device.c_str(), O_RDWR); //device is a C++ std::string, but Linux's open() expects a C-style string (const char*)

    if (fd_ < 0) // if it fails to open, return false
    {
        return false;
    }

     if (!drmIsMaster(fd_)) //Check that the application is DRM Master.
    {
        close();
        return false;
    }
    
    //DRM Resource discovery
    drmModeRes* resources = drmModeGetResources(fd_);

    if (resources == nullptr)
    {
        close();
        return false;
    }

    //Diagnostics output
    std::cout << "Connectors: "
              << resources->count_connectors
              << '\n';

    std::cout << "CRTCs: "
              << resources->count_crtcs
              << '\n';

    //DRM Connector discovery
    drmModeConnector* connector =
        drmModeGetConnector(fd_, resources->connectors[0]);

    if (connector == nullptr)
    {
        close();
        return false;
    }

    width_ = connector->modes[0].hdisplay;
    height_ = connector->modes[0].vdisplay; 

    drmModeEncoder* encoder =
        drmModeGetEncoder(fd_, connector->encoder_id);

    if (encoder == nullptr)
    {
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
        close();
        return false;
    }

    crtcId_ = encoder->crtc_id;

    //Get the initial FrameBuffer. So the Application can revert back to that upon closing
    drmModeCrtc* crtc =
        drmModeGetCrtc(fd_, crtcId_);

    if (crtc == nullptr)
    {
        drmModeFreeEncoder(encoder);
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
        close();
        return false;
    }

    originalFramebufferId_ = crtc->buffer_id;
    //------------------------------------------------------

    //Create and map the application framebuffers
    for (int i = 0; i < 2; ++i)
    {
        //Calculate the buffer size needed
        drm_mode_create_dumb createRequest = {};

        createRequest.width =
            static_cast<uint32_t>(width_);

        createRequest.height =
            static_cast<uint32_t>(height_);

        createRequest.bpp = 32;

        if (drmIoctl(
                fd_,
                DRM_IOCTL_MODE_CREATE_DUMB,
                &createRequest) < 0)
        {
            drmModeFreeCrtc(crtc);
            drmModeFreeEncoder(encoder);
            drmModeFreeConnector(connector);
            drmModeFreeResources(resources);
            close();
            return false;
        }

        //Store dumb buffer information
        buffers_[i].dumbBufferHandle =
            createRequest.handle;

        buffers_[i].size =
            createRequest.size;

        //Get buffer offset for mapping
        drm_mode_map_dumb mapRequest = {};
        mapRequest.handle =
            buffers_[i].dumbBufferHandle;

        if (drmIoctl(
                fd_,
                DRM_IOCTL_MODE_MAP_DUMB,
                &mapRequest) < 0)
        {
            drmModeFreeCrtc(crtc);
            drmModeFreeEncoder(encoder);
            drmModeFreeConnector(connector);
            drmModeFreeResources(resources);
            close();
            return false;
        }

        //Map dumb buffer
        buffers_[i].memory = mmap(
            nullptr,
            buffers_[i].size,
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd_,
            mapRequest.offset);

        if (buffers_[i].memory == MAP_FAILED) //if mapping fails, then cleanup
        {
            buffers_[i].memory = nullptr;

            drmModeFreeCrtc(crtc);
            drmModeFreeEncoder(encoder);
            drmModeFreeConnector(connector);
            drmModeFreeResources(resources);
            close();
            return false;
        }

        //Create DRM framebuffer
        uint32_t handles[4] = {};
        uint32_t pitches[4] = {};
        uint32_t offsets[4] = {};

        handles[0] =
            buffers_[i].dumbBufferHandle;

        pitches[0] =
            createRequest.pitch;

        offsets[0] = 0;

        if (drmModeAddFB2(
                fd_,
                static_cast<uint32_t>(width_),
                static_cast<uint32_t>(height_),
                DRM_FORMAT_XRGB8888,
                handles,
                pitches,
                offsets,
                &buffers_[i].framebufferId,
                0) != 0)
        {
            drmModeFreeCrtc(crtc);
            drmModeFreeEncoder(encoder);
            drmModeFreeConnector(connector);
            drmModeFreeResources(resources);
            close();
            return false;
        }
        
        //Initialize framebuffer to 0 (Black)
        std::memset(
            buffers_[i].memory,
            0,
            buffers_[i].size);
    }



    //Display the first framebuffer
    uint32_t connectorId = connector->connector_id;

    if (drmModeSetCrtc(
            fd_,
            crtcId_,
            buffers_[0].framebufferId,
            0,
            0,
            &connectorId,
            1,
            &connector->modes[0]) != 0)
    {
        drmModeFreeEncoder(encoder);
        drmModeFreeCrtc(crtc);
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
        close();
        return false;
    }

    activeBufferIndex_ = 0; //FB0 is now being displayed
    pageFlipPending_ = false;

    //Diagnostics output
    std::cout << "Connector ID: "
              << connector->connector_id
              << '\n';

    std::cout << "Encoder ID: "
              << connector->encoder_id
              << '\n';

    std::cout << "Encoder CRTC ID: "
              << crtcId_
              << '\n';

    std::cout << "Current framebuffer ID: "
              << originalFramebufferId_
              << '\n';
    
    for (int i = 0; i < 2; ++i)
    {
        std::cout << "Dumb buffer "
                  << i
                  << " size: "
                  << buffers_[i].size
                  << '\n';

        std::cout << "Application framebuffer "
                  << i
                  << " ID: "
                  << buffers_[i].framebufferId
                  << '\n';
    }

    std::cout << "Connection status: "
              << connector->connection
              << '\n';

    std::cout << "Modes: "
              << connector->count_modes
              << '\n';

    std::cout << "Mode width: "
              << connector->modes[0].hdisplay
              << '\n';

    std::cout << "Mode height: "
              << connector->modes[0].vdisplay
              << '\n';

    std::cout << "Mode name: "
              << connector->modes[0].name
              << '\n';

    //Cleanup after sucsessfull frameBuffer open()
    drmModeFreeEncoder(encoder);
    drmModeFreeCrtc(crtc);
    drmModeFreeConnector(connector);
    drmModeFreeResources(resources); //Resources contains lists of IDs for "connectors, CRTCs, encoders, planes etc"

    return true;
}

//defines the function close() as part of Framebuffer class
void Framebuffer::close()
//Document what close does
{
    //Restore the framebuffer that was active before the application started.
    if (originalFramebufferId_ != 0)
    {
        drmModeSetCrtc(
            fd_,
            crtcId_,
            originalFramebufferId_,
            0,
            0,
            nullptr,
            0,
            nullptr);

        originalFramebufferId_ = 0;
    }

    //Cleanup application buffers
    for (int i = 0; i < 2; ++i)
    {
        //Remove DRM framebuffer
        if (buffers_[i].framebufferId != 0)
        {
            std::cout << "Removing application framebuffer "
                      << i
                      << ": "
                      << buffers_[i].framebufferId
                      << '\n';

            drmModeRmFB(
                fd_,
                buffers_[i].framebufferId);

            buffers_[i].framebufferId = 0;
        }

        //Unmap dumb buffer
        if (buffers_[i].memory != nullptr)
        {
            munmap(
                buffers_[i].memory,
                buffers_[i].size);

            buffers_[i].memory = nullptr;
        }

        //Destroy dumb buffer
        if (buffers_[i].dumbBufferHandle != 0)
        {
            drm_mode_destroy_dumb destroyRequest = {};

            destroyRequest.handle =
                buffers_[i].dumbBufferHandle;

            drmIoctl(
                fd_,
                DRM_IOCTL_MODE_DESTROY_DUMB,
                &destroyRequest);

            buffers_[i].dumbBufferHandle = 0;
        }

        buffers_[i].size = 0;
    }

    std::cout << "Application framebuffers cleanup complete.\n";
    
    //Close DRM device
    if (fd_ >= 0)
    {
        ::close(fd_);
        fd_ = -1;
    }
}

//Query functions that provide information about the framebuffer.
bool Framebuffer::isOpen() const
{
    return fd_ >= 0;
}

void Framebuffer::pageFlipHandler(int,unsigned int,unsigned int,unsigned int,void* data)
{
    auto* framebuffer =
        static_cast<Framebuffer*>(data);

    framebuffer->pageFlipPending_ = false;
}

bool Framebuffer::present(bool newFrameAvailable)
{
    if (!newFrameAvailable)
    {
        return true;
    }

    if (pageFlipPending_)
    {
        return false;
    }

    const int nextBufferIndex =
        1 - activeBufferIndex_;

    pageFlipPending_ = true;

    if (drmModePageFlip(
            fd_,
            crtcId_,
            buffers_[nextBufferIndex].framebufferId,
            DRM_MODE_PAGE_FLIP_EVENT,
            this) != 0)
    {
        pageFlipPending_ = false;
        return false;
    }

    drmEventContext eventContext = {};

    eventContext.version = DRM_EVENT_CONTEXT_VERSION;
    eventContext.page_flip_handler =
        pageFlipHandler;

    while (pageFlipPending_)
    {
        pollfd pollFd = {};
        pollFd.fd = fd_;
        pollFd.events = POLLIN;

        int result = poll(
            &pollFd,
            1,
            1000); //1 second timeout

        if (result < 0) //failed, typically because of an error or signal interruption.
        {
            pageFlipPending_ = false;
            return false;
        }

        if (result == 0) //timed out normally.
        {
            pageFlipPending_ = false;
            return false;
        }

        if (pollFd.revents & POLLIN)
        {
            if (drmHandleEvent(
                    fd_,
                    &eventContext) != 0)
            {
                pageFlipPending_ = false;
                return false;
            }
        }
    }

    activeBufferIndex_ = nextBufferIndex;

    return true;
}

int Framebuffer::width() const
{
    return width_;
}

int Framebuffer::height() const

{
    return height_;
}

int Framebuffer::backBufferIndex() const
{
    return 1 - activeBufferIndex_;
}

void* Framebuffer::bufferMemory(int index)
{
    return buffers_[index].memory;
}