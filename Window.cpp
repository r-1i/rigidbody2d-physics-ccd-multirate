#include "Window.h"
#include <stdexcept>
#include <SDL2/SDL2_gfxPrimitives.h>


Window::Window(int width, int height) : m_width(width), m_height(height)
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


void Window::AddToBuffer(PhysObject* physObject)
{
	m_buffer.push_back(physObject);
}

void Window::Render()
{
	for (const auto& body : m_buffer) {
		switch (body->shapeType) {
		case ShapeType::Circle:
			filledCircleRGBA(m_renderer, body->pos.x, body->pos.y, body->radius, 255, 255, 255, 255);
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

void Window::ClearBuffer()
{
	m_buffer.clear();
}
