#pragma once

#include <vector>

#include "PhysObject.h"
#include <SDL2/SDL.h>
#include <memory>

class Window {
public:
	Window(int width, int height, std::vector<std::shared_ptr<PhysObject>>* pObjects);
	void Render();

private:
	SDL_Renderer* m_renderer;
	SDL_Window* m_window;
	std::vector<std::shared_ptr<PhysObject>>* pObjects;
	int m_width;
	int m_height;

	void Init();
};