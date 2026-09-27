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

	pixels[0] = 0x00FF0000;

	framebuffer.present(true);
    

    sleep(5);

    return 0;
}