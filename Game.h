#pragma once
#include "Window.h"
#include "Simulation.h"
#include <memory>
#define REFRESH_RATE_60 60;
#define REFRESH_RATE_240 240;

enum class SceneState {
	DiscreteDemo,
	CCDScene,
	MultiRate1,
	MultiRate2,
	StackDiscrete,
	StackCCD,
	Count
};

class Game {
public:
	Game();
	void Run();
	int GetPhysicsFrameCount() const;
	float GetTime() const;
	int GetRefreshRate() const { return currentRefreshRate; }
	SceneState GetCurrentState() const { return currentState; }
	CollisionMethod GetCurrentCollisionMethod() const { return collisionMethod; }
	float GetPhysicsStepTimeMs() const { return lastPhysicsStepTimeMs; }


private:
	float lastPhysicsStepTimeMs = 0.0f;

	SceneState currentState = SceneState::DiscreteDemo;
	CollisionMethod collisionMethod = CollisionMethod::Discrete;
	void LoadScene(SceneState state);
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
	void InitSceneDiscrete();
	void InitSceneCCD();
	void InitSceneMultiRate();
	void InitSceneMultiRate2();
	void InitSceneStackDiscrete();
	void InitSceneStackCCD();
	void BuildStackScene();
};