#include "Framebuffer.hpp"

#include <iostream>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <chrono>

int main()
{
    Framebuffer framebuffer; //Create a framebuffer handler

    if (!framebuffer.open()) //Open the framebuffer
    {
        std::cerr << "Failed to open framebuffer.\n";
        return 1;
    }

	const int screenWidth = framebuffer.width();
	const int screenHeight = framebuffer.height();

	//create back buffer in RAM
	std::vector<std::uint32_t> backBuffer(
		static_cast<std::size_t>(screenWidth) *
		static_cast<std::size_t>(screenHeight));
	
	//Write Debug information to console
    std::cout << "Framebuffer opened successfully.\n";
    std::cout << "Resolution: "
              << framebuffer.width()
              << "x"
              << framebuffer.height()
              << '\n';

	std::cout << "Virtual width: "
          	  << framebuffer.virtualWidth()
          	  << '\n';

    std::cout << "Bits per pixel: "
              << framebuffer.bitsPerPixel()
              << '\n';

	//############################################################
	// Drawing part of main	
	//############################################################

	// fill screen black
	//framebuffer.fill(0xFF000000);		//Using the frameBuffer class
	std::fill(							//Using the back buffer in RAM
		backBuffer.begin(),
		backBuffer.end(),
		0xFF000000);

	const int squareSize = 20;
	int x = 100;
	int y = 140;

	for (int newX = 100; newX <= 600; newX += 10)
	{
		// Fill the back buffer with black.
		std::fill(
			backBuffer.begin(),
			backBuffer.end(),
			0xFF000000);
		
		// Draw the square into the back buffer.
		for (int currentY = y;
			currentY < y + squareSize;
			++currentY)
		{
			for (int currentX = newX;
				currentX < newX + squareSize;
				++currentX)
			{
				backBuffer[
					static_cast<std::size_t>(currentY) *
					static_cast<std::size_t>(screenWidth) +
					static_cast<std::size_t>(currentX)
				] = 0xFFFF0000;
			}
		}
	
	
/*
	std::vector<std::uint32_t> savedPixels;

	int x = 100;

	// Save the background at the first position.
	framebuffer.saveRegion(
		x,
		140,
		20,
		20,
		savedPixels);

	// Draw the first rectangle.
	framebuffer.drawRect(
		x,
		140,
		20,
		20,
		0xFFFF0000);

	for (int newX = 110; newX <= 600; newX += 10)
	{
		framebuffer.waitForVSync();

		usleep(50000);

		// Remove the rectangle from its old position.
		framebuffer.restoreRegion(
			x,
			140,
			20,
			20,
			savedPixels);

		// Move to the new position.
		x = newX;

		// Save the background at the new position.
		framebuffer.saveRegion(
			x,
			140,
			20,
			20,
			savedPixels);

		// Draw the rectangle at the new position.
		framebuffer.drawRect(
			x,
			140,
			20,
			20,
			0xFFFF0000);
	}
*/
	// Display the completed frame.
	auto start = std::chrono::steady_clock::now();

	framebuffer.drawBuffer(backBuffer);

	auto end = std::chrono::steady_clock::now();

	auto elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(
			end - start);

	std::cout << "drawBuffer: "
			<< elapsed.count()
			<< " us\n";


	// Slow the animation down so we can see it.
	usleep(50000);

	}
	//############################################################
	//End of drawing
	//############################################################

    std::cout << "Pixel written.\n";

    sleep(5);

    return 0;
}