#pragma once

#include <vector>

#include "PhysObject.h"
#include <SDL2/SDL.h>
#include <memory>

class Game;

class Window {
public:
	Window(int width, int height, std::vector<std::shared_ptr<PhysObject>>* pObjects, const Game& game);
	void Render();

private:
	SDL_Renderer* m_renderer;
	SDL_Window* m_window;
	std::vector<std::shared_ptr<PhysObject>>* pObjects;
	int m_width;
	int m_height;
	const Game& game;

	void Init();
};