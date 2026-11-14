#pragma once
#include "Window.h"
#include "Simulation.h"
#include <memory>

class Game {
public:
	Game();
	void Run();

private:
	Window window;
	Simulation simulation;
	std::vector<std::shared_ptr<PhysObject>> objects;

	Uint64 freq;
	Uint64 prevCounter;

	float GetDeltaTime();
};