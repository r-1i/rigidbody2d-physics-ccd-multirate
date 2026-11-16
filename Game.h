#pragma once
#include "Window.h"
#include "Simulation.h"
#include <memory>
#define REFRESH_RATE_60 60;
#define REFRESH_RATE_240 240;

class Game {
public:
	Game();
	void Run();
	int GetPhysicsFrameCount() const;
	float GetTime() const;
	int GetRefreshRate() const { return currentRefreshRate; }

private:
	Window window;
	Simulation simulation;
	std::vector<std::shared_ptr<PhysObject>> objects;

	Uint64 freq;
	Uint64 prevCounter;

	bool isPaused = false;
	bool stepNextFrame = false;

	int physicsFrameCount = 0;
	float timeFromStart = 0.0f;
	int currentRefreshRate = 0; 

	float GetDeltaTime();
	void InitScene1();
	void InitScene2();
	void InitScene3();
};