#include "Framebuffer.hpp"

#include <iostream>
#include <unistd.h>

int main()
{
    Framebuffer framebuffer; //Create a framebuffer handler

    if (!framebuffer.open()) //Open the framebuffer
    {
        std::cerr << "Failed to open framebuffer.\n";
        return 1;
    }

    std::cout << "Framebuffer opened successfully.\n";
    std::cout << "Resolution: "
              << framebuffer.width()
              << "x"
              << framebuffer.height()
              << '\n';

    std::cout << "Bits per pixel: "
              << framebuffer.bitsPerPixel()
              << '\n';

// Drawing part of main		
	for (int x = 0; x <= 600; x += 10)
	{
    	// Clear the previous frame.
    	framebuffer.fill(0xFF000000); // Black

    	// Draw the rectangle at its new position.
    	framebuffer.drawRect(
        	x,		//x
        	140,	//y
        	200,	//width
        	200,	//height
        	0xFFFF0000); // Red

    	// Wait 50 milliseconds.
    	usleep(50000);
	}

    std::cout << "Pixel written.\n";

    sleep(5);

    return 0;
}