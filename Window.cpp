#include "Window.h"
#include <stdexcept>
#include <SDL2/SDL2_gfxPrimitives.h>


Window::Window(int width, int height, std::vector<std::shared_ptr<PhysObject>>* pObjects) 
	: m_width(width), m_height(height), pObjects(pObjects)
{
	Init();
}

void Window::Init()
{
	m_window = SDL_CreateWindow("Rigidbody 2D Simulation: CCD & Multi-rate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_SHOWN);
	if (m_window == nullptr) {
		std::string msg = "Error while creating window" + std::string(SDL_GetError());
		throw std::domain_error(msg);
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_renderer == nullptr) {
		std::string msg = "Error while creating renderer" + std::string(SDL_GetError());
		throw std::domain_error(msg);
	}


}

void Window::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
	SDL_RenderClear(m_renderer);

	for (const auto& body : *pObjects) {
		switch (body->shapeType) {
		case ShapeType::Circle:
			filledCircleRGBA(m_renderer, body->pos.x, body->pos.y, body->radius, 0, 255, 0, 255);
			break;
		case ShapeType::AABB:
			SDL_FRect rect {body->pos.x - body->halfExtents.x, body->pos.y - body->halfExtents.y,
					2.f * body->halfExtents.x, 2.f * body->halfExtents.y };
			SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
			SDL_RenderFillRectF(m_renderer, &rect);
			break;
		}
	}

	SDL_RenderPresent(m_renderer);
}
