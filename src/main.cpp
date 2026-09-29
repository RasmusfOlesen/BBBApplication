#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "Gauge.hpp"
#include "TextRenderer.hpp"

#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <thread>
#include <chrono>



int main()
{
	

    Framebuffer framebuffer; //Call the framebuffer class

    if (!framebuffer.open()) //Open the framebuffer
    {
        std::cerr << "Failed to open framebuffer.\n";
        return 1;
    }

	//Write Debug information to console
    std::cout << "Framebuffer opened successfully.\n";

    std::cout << "Resolution: "
              << framebuffer.width()
              << "x"
              << framebuffer.height()
              << '\n';

	//What buffer shal we write to
	int backBufferIndex = framebuffer.backBufferIndex();

	//Where is the active back buffer
	std::uint32_t* pixels =
		static_cast<std::uint32_t*>(
			framebuffer.bufferMemory(backBufferIndex));

	Renderer renderer(			//Call the Renderer class
		pixels,
		framebuffer.width(),
		framebuffer.height());

	TextRenderer textRenderer(
		pixels,
		framebuffer.width(),
		framebuffer.height());

	Gauge gauge(
    renderer,
    128,
    128);

	const RegionId needleRegionBuffer0 = 0;
	const RegionId needleRegionBuffer1 = 1;

	double needleAngle = 55.0;
	double needleStep = -1.0;

	//Draw Initial background
	renderer.fill(0x00000000); //All black

	gauge.drawBackground();

	gauge.setNeedleAngle(needleAngle);

	RenderRegion region =
    	gauge.needleRegion();

	renderer.saveRegion(
    needleRegionBuffer0,
    region.x,
    region.y,
    region.width,
    region.height);

	gauge.drawNeedle();

	framebuffer.present(true);

	backBufferIndex = framebuffer.backBufferIndex();

	renderer.setTarget(
		static_cast<std::uint32_t*>(
			framebuffer.bufferMemory(
				backBufferIndex)));

	renderer.fill(0x00000000);

	gauge.drawBackground();

	renderer.saveRegion(
		needleRegionBuffer1,
		region.x,
		region.y,
		region.width,
		region.height);

	gauge.drawNeedle();

	bool running = true;

	while (running)
	{
		needleAngle += needleStep;

		if (needleAngle <= -45.0)
		{
			needleAngle = -45.0;
			needleStep = 1.0;
		}
		else if (needleAngle >= 55.0)
		{
			//needleAngle = 55.0;
			needleStep = -1.0;
			running = false;
		}

		int backBufferIndex =
        	framebuffer.backBufferIndex();

		renderer.setTarget(
			static_cast<std::uint32_t*>(
				framebuffer.bufferMemory(
					backBufferIndex)));
		
		RegionId regionId =
			backBufferIndex == 0
				? needleRegionBuffer0
				: needleRegionBuffer1;
					
		renderer.restoreRegion(regionId);

		gauge.setNeedleAngle(needleAngle);

		RenderRegion region =
        	gauge.needleRegion();
		
		renderer.saveRegion(
			regionId,
			region.x,
			region.y,
			region.width,
			region.height);

		gauge.drawNeedle();

		framebuffer.present(true);

		std::this_thread::sleep_for(
			std::chrono::milliseconds(20));
	}

	//sleep(10);

    return 0;
}