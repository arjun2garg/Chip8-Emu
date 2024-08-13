#include <chrono>
#include <iostream>

#include <SDL2/SDL.h>

#include "chip8.h"
#include "chip8.cpp"
#include "platform.h"
#include "platform.cpp"



int main(int argc, char** argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = std::stoi(argv[1]);
	int cycleDelay = std::stoi(argv[2]);
	char const* romFilename = argv[3];

	Platform platform("CHIP-8 Emulator", 64 * videoScale, 32 * videoScale, 64, 32);

	Chip8 chip8;
	chip8.load(romFilename);

	int videoPitch = sizeof(chip8.display[0]) * 64;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.ProcessInput(chip8.keys);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.cycle();

			platform.Update(chip8.display, videoPitch);
		}
	}

	return 0;
}