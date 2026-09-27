#include "Framebuffer.hpp"
#include "Renderer.hpp"

#include <cstdint>
#include <iostream>
#include <unistd.h>

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
	int backBufferIndex =
    framebuffer.backBufferIndex();

	//Where is the active back buffer
	std::uint32_t* pixels =
		static_cast<std::uint32_t*>(
			framebuffer.bufferMemory(backBufferIndex));

	Renderer renderer(			//Call the Renderer class
		pixels,
		framebuffer.width(),
		framebuffer.height());

	renderer.fill(0x00000000);

	renderer.drawRect(
		100,
		100,
		100,
		100,
		0x00FF0000);

framebuffer.present(true);	

	for (int x = 0; x < framebuffer.width() - 100; x += 10)
	{
		auto drawStart =
    		std::chrono::steady_clock::now();

		renderer.fill(0x00000000);

		renderer.drawRect(
			x,
			100,
			100,
			100,
			0x00FF0000);

		auto drawEnd =
    		std::chrono::steady_clock::now();

		framebuffer.present(true);

		auto presentEnd =
    		std::chrono::steady_clock::now();

		int nextBackBufferIndex =
        	framebuffer.backBufferIndex();

		renderer.setTarget(
			static_cast<std::uint32_t*>(
				framebuffer.bufferMemory(
					nextBackBufferIndex)));

		auto drawTime =
			std::chrono::duration_cast<
				std::chrono::microseconds>(
					drawEnd - drawStart)
				.count();

		auto presentTime =
			std::chrono::duration_cast<
				std::chrono::microseconds>(
					presentEnd - drawEnd)
				.count();

		std::cout << "Draw: "
				  << drawTime
				  << " us, Present: "
				  << presentTime
				  << " us\n";
		
	}

    return 0;
}