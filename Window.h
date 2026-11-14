#pragma once

#include <vector>

#include "PhysObject.h"
#include <SDL2/SDL.h>

class Window {
public:
	Window(int width, int height);
	void AddToBuffer(PhysObject* physObject);
	void Render();
	void ClearBuffer();

private:
	SDL_Renderer* m_renderer;
	SDL_Window* m_window;
	std::vector<PhysObject*> m_buffer;
	int m_width;
	int m_height;

	void Init();
};