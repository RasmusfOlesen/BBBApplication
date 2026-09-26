#include <xf86drm.h>
#include <xf86drmMode.h>
#include <drm.h>
#include <drm_fourcc.h>
#include <sys/mman.h>
#include <iostream>
#include <cstdint>
#include <poll.h>

static void pageFlipHandler(
    int fd,
    unsigned int frame,
    unsigned int sec,
    unsigned int usec,
    void* data)
{
    std::cout << "Page flip completed.\n";
}

int main()
{
    int fd = drmOpen("tilcdc", nullptr);

    if (fd < 0)
    {
        std::cerr << "Could not open DRM device.\n";
        return 1;
    }

    std::cout << "DRM device opened successfully.\n";

    drmEventContext eventContext{};

    eventContext.version = DRM_EVENT_CONTEXT_VERSION;
    eventContext.page_flip_handler = pageFlipHandler;

    if (drmIsMaster(fd))
    {
        std::cout << "Application is DRM master.\n";
    }
    else
    {
        std::cout << "Application is NOT DRM master.\n";
    }

    //Create first DumbBuffer
    drm_mode_create_dumb createRequest{};

    createRequest.width = 800;
    createRequest.height = 480;
    createRequest.bpp = 32;

    if (drmIoctl(
            fd,
            DRM_IOCTL_MODE_CREATE_DUMB,
            &createRequest) == 0)
    {
        std::cout << "Dumb buffer created successfully.\n";
        std::cout << "  Handle: "
                  << createRequest.handle
                  << '\n';

        std::cout << "  Pitch: "
                  << createRequest.pitch
                  << '\n';

        std::cout << "  Size: "
                  << createRequest.size
                  << '\n';
    }

    //Create Second DumbBuffer
    drm_mode_create_dumb createRequest2{};

    createRequest2.width = 800;
    createRequest2.height = 480;
    createRequest2.bpp = 32;

    if (drmIoctl(
            fd,
            DRM_IOCTL_MODE_CREATE_DUMB,
            &createRequest2) == 0)
    {
        std::cout << "Second dumb buffer created successfully.\n";
        std::cout << "  Handle: " << createRequest2.handle << '\n';
        std::cout << "  Pitch: " << createRequest2.pitch << '\n';
        std::cout << "  Size: " << createRequest2.size << '\n';
    }

    //Maprequest for first DumbBuffer
    drm_mode_map_dumb mapRequest{};

    mapRequest.handle = createRequest.handle;

    if (drmIoctl(
            fd,
            DRM_IOCTL_MODE_MAP_DUMB,
            &mapRequest) == 0)
    {
        std::cout << "Dumb buffer mapped successfully.\n";

        std::cout << "  Offset: "
                  << mapRequest.offset
                  << '\n';
    }
    //Execute first Maprequest
    void* dumbBuffer = mmap(
        nullptr,
        createRequest.size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        mapRequest.offset);
    //Initialize first dumbBuffer to red and make first pixel red
    if (dumbBuffer != MAP_FAILED)
    {
        std::cout << "Dumb buffer mmap successful.\n";

        auto* pixels =
            static_cast<std::uint32_t*>(dumbBuffer);

        const std::size_t pixelCount = 800 * 480;

        for (std::size_t i = 0; i < pixelCount; ++i)
        {
            pixels[i] = 0xFFFF0000;
        }

        pixels[0] = 0xFFFF0000;

        std::cout << "Pixel written to dumb buffer.\n";
    }

    //Maprequest for second DumbBuffer
    drm_mode_map_dumb mapRequest2{};

    mapRequest2.handle = createRequest2.handle;

    if (drmIoctl(
            fd,
            DRM_IOCTL_MODE_MAP_DUMB,
            &mapRequest2) == 0)
    {
        std::cout << "Second dumb buffer mapped successfully.\n";
        std::cout << "  Offset: " << mapRequest2.offset << '\n';
    }
    //Execute second maprequest
    void* dumbBuffer2 = mmap(
        nullptr,
        createRequest2.size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        mapRequest2.offset);
    //Initialize second dumbBuffer to green and make first pixel green
    if (dumbBuffer2 != MAP_FAILED)
    {
        auto* pixels2 =
            static_cast<std::uint32_t*>(dumbBuffer2);

        const std::size_t pixelCount2 = 800 * 480;

        for (std::size_t i = 0;
             i < pixelCount2;
             ++i)
        {
            pixels2[i] = 0xFF00FF00;
        }

        pixels2[0] = 0xFF00FF00;

        std::cout << "Second dumb buffer mmap successful.\n";
    }

    //Map first dumbBuffer to first frameBuffer
    uint32_t framebufferId = 0;

    uint32_t handles[4] = {};
    uint32_t pitches[4] = {};
    uint32_t offsets[4] = {};

    handles[0] = createRequest.handle;
    pitches[0] = createRequest.pitch;
    offsets[0] = 0;

    if (drmModeAddFB2(
            fd,
            800,
            480,
            DRM_FORMAT_XRGB8888,
            handles,
            pitches,
            offsets,
            &framebufferId,
            0) == 0)
    {
        std::cout << "DRM framebuffer created successfully.\n";

        std::cout << "  Framebuffer ID: "
                  << framebufferId
                  << '\n';
    }

    //Map second dumbBuffer to second framebuffer
    uint32_t framebufferId2 = 0;

    uint32_t handles2[4] = {};
    uint32_t pitches2[4] = {};
    uint32_t offsets2[4] = {};

    handles2[0] = createRequest2.handle;
    pitches2[0] = createRequest2.pitch;
    offsets2[0] = 0;

    if (drmModeAddFB2(
            fd,
            800,
            480,
            DRM_FORMAT_XRGB8888,
            handles2,
            pitches2,
            offsets2,
            &framebufferId2,
            0) == 0)
    {
        std::cout << "Second DRM framebuffer created successfully.\n";
        std::cout << "  Framebuffer ID: "
                  << framebufferId2
                  << '\n';
    }
    else
    {
        std::cout << "Second DRM framebuffer creation failed.\n";
    }

    //PollDescriptor setup
    pollfd pollDescriptor{};

    pollDescriptor.fd = fd;
    pollDescriptor.events = POLLIN;
    
    //Page flipping loop
    for (int i = 0; i < 120; ++i)
    {
        //Pageflipping to FB40
        if (drmModePageFlip(
                fd,
                35,
                framebufferId,
                DRM_MODE_PAGE_FLIP_EVENT,
                nullptr) == 0)
        {
            std::cout << "Page flip to FB40 scheduled.\n";
        }
        else
        {
            std::cout << "Page flip to FB40 failed.\n";
        }

        if (poll(
                &pollDescriptor,
                1,
                1000) > 0)
        {
            drmHandleEvent(fd, &eventContext);
        }
        else
        {
            std::cout << "Timed out waiting for page flip to FB40.\n";
        }
    
        //Page flipping to FB41
        if (drmModePageFlip(
                fd,
                35,
                framebufferId2,
                DRM_MODE_PAGE_FLIP_EVENT,
                nullptr) == 0)
        {
            std::cout << "Page flip to FB41 scheduled.\n";
        }
        else
        {
            std::cout << "Page flip to FB41 failed.\n";
        }

        if (poll(
                &pollDescriptor,
                1,
                1000) > 0)
        {
            drmHandleEvent(fd, &eventContext);
        }
        else
        {
            std::cout << "Timed out waiting for page flip to FB41.\n";
        }

        //Page flipping to FB40
        if (drmModePageFlip(
                fd,
                35,
                framebufferId,
                DRM_MODE_PAGE_FLIP_EVENT,
                nullptr) == 0)
        {
            std::cout << "Page flip to FB40 scheduled again.\n";
        }
        else
        {
            std::cout << "Page flip to FB40 failed.\n";
        }

        if (poll(
                &pollDescriptor,
                1,
                1000) > 0)
        {
            drmHandleEvent(fd, &eventContext);
        }
        else
        {
            std::cout << "Timed out waiting for page flip to FB40.\n";
        }
    }

    //###########################################################################################
    //Diagnostics / Discovery code
    uint64_t dumbBufferSupport = 0;

    if (drmGetCap(
            fd,
            DRM_CAP_DUMB_BUFFER,
            &dumbBufferSupport) == 0)
    {
        std::cout << "Dumb buffer support: "
                  << dumbBufferSupport
                  << '\n';
    }

    drmSetClientCap(
        fd,
        DRM_CLIENT_CAP_UNIVERSAL_PLANES,
        1);

    drmModeRes* resources =
        drmModeGetResources(fd);

    if (resources != nullptr)
    {
        std::cout << "Connectors: "
                  << resources->count_connectors
                  << '\n';

        std::cout << "CRTCs: "
                  << resources->count_crtcs
                  << '\n';

        for (int i = 0;
             i < resources->count_connectors;
             ++i)
        {
            uint32_t connectorId =
                resources->connectors[i];

            std::cout << "Connector ID: "
                      << connectorId
                      << '\n';

            drmModeConnector* connector =
                drmModeGetConnector(
                    fd,
                    connectorId);

            if (connector != nullptr)
            {
                std::cout << "Connector status: "
                          << connector->connection
                          << '\n';

                std::cout << "Modes: "
                          << connector->count_modes
                          << '\n';

                for (int j = 0;
                     j < connector->count_modes;
                     ++j)
                {
                    std::cout
                        << "Mode: "
                        << connector->modes[j].name
                        << ' '
                        << connector->modes[j].hdisplay
                        << 'x'
                        << connector->modes[j].vdisplay
                        << " @ "
                        << connector->modes[j].vrefresh
                        << " Hz\n";
                }

                drmModeFreeConnector(connector);
            }
        }

        for (int i = 0;
             i < resources->count_crtcs;
             ++i)
        {
            uint32_t crtcId =
                resources->crtcs[i];

            std::cout << "CRTC ID: "
                      << crtcId
                      << '\n';

            drmModeCrtc* crtc =
                drmModeGetCrtc(
                    fd,
                    crtcId);

            if (crtc != nullptr)
            {
                std::cout << "Current framebuffer ID: "
                          << crtc->buffer_id
                          << '\n';

                std::cout << "Position: "
                          << crtc->x
                          << ", "
                          << crtc->y
                          << '\n';

                std::cout << "Size: "
                          << crtc->width
                          << "x"
                          << crtc->height
                          << '\n';

                drmModeFreeCrtc(crtc);
            }
        }

        drmModeFreeResources(resources);
    }

    drmModeFB2* framebuffer =
        drmModeGetFB2(fd, 38);

    if (framebuffer != nullptr)
    {
        std::cout << "Framebuffer ID: "
                  << framebuffer->fb_id
                  << '\n';

        std::cout << "Size: "
                  << framebuffer->width
                  << "x"
                  << framebuffer->height
                  << '\n';

        std::cout << "Pitch: "
                  << framebuffer->pitches[0]
                  << '\n';

        std::cout << "Format: "
                  << static_cast<char>(
                         framebuffer->pixel_format & 0xFF)
                  << static_cast<char>(
                         (framebuffer->pixel_format >> 8) & 0xFF)
                  << static_cast<char>(
                         (framebuffer->pixel_format >> 16) & 0xFF)
                  << static_cast<char>(
                         (framebuffer->pixel_format >> 24) & 0xFF)
                  << '\n';

        drmModeFreeFB2(framebuffer);
    }

    drmModePlaneRes* planeResources =
        drmModeGetPlaneResources(fd);

    if (planeResources != nullptr)
    {
        std::cout << "Planes: "
                  << planeResources->count_planes
                  << '\n';

        for (uint32_t i = 0;
             i < planeResources->count_planes;
             ++i)
        {
            uint32_t planeId =
                planeResources->planes[i];

            drmModePlane* plane =
                drmModeGetPlane(
                    fd,
                    planeId);

            if (plane != nullptr)
            {
                std::cout << "Plane ID: "
                          << planeId
                          << '\n';

                std::cout << "  Current CRTC: "
                          << plane->crtc_id
                          << '\n';

                std::cout << "  Current framebuffer: "
                          << plane->fb_id
                          << '\n';

                drmModeFreePlane(plane);
            }
        }

        drmModeFreePlaneResources(
            planeResources);
    }

    return 0;
}
