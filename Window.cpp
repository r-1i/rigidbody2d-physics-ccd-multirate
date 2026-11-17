#include "Window.h"
#include "Game.h"
#include <stdexcept>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <string>
#include <format>

Window::Window(int width, int height, std::vector<std::shared_ptr<PhysObject>>* pObjects, const Game& game)
	: m_width(width), m_height(height), pObjects(pObjects), game(game)
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
		switch (body->GetShapeType()) {
		case ShapeType::Circle:
			filledCircleRGBA(m_renderer, body->pos.x, body->pos.y , body->radius, 255, 255, 255, 255);
			break;
		case ShapeType::AABB:
			SDL_FRect rect {body->pos.x - body->halfExtents.x, body->pos.y - body->halfExtents.y,
					2.f * body->halfExtents.x, 2.f * body->halfExtents.y };
			SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
			SDL_RenderFillRectF(m_renderer, &rect);
			break;
		}
	}

	std::string textPhysicsFrames = "Physics Frames:  " + std::to_string(game.GetPhysicsFrameCount());
	std::string textTimeFromStart = "Time from start: " + std::format("{:.2f}", game.GetTime());
	std::string refreshRate =		"Refresh rate:    " + std::to_string(game.GetRefreshRate());
	std::string mode = "";
	switch (game.GetCurrentState()) {
	case SceneState::DiscreteDemo: 
		mode = "Discrete Demo";
		break;
	case SceneState::CCDScene:
		mode = "CCD Scene";
		break;
	case SceneState::MultiRate1:
		mode = "Multi-rate 1";
		break;
	case SceneState::MultiRate2:
		mode = "Multi-rate 2";
		break;
	case SceneState::StackDiscrete:
		mode = "Stack Discrete";
		break;
	case SceneState::StackCCD:
		mode = "Stack CCD";
		break;
	}
	std::string simulationMode = "Simulation mode :" + mode;

	std::string colMethod = "";
	switch (game.GetCurrentCollisionMethod()) {
	case CollisionMethod::Discrete:
		colMethod = "Discrete";
		break;
	case CollisionMethod::CCD:
		colMethod = "CCD";
		break;
	case CollisionMethod::MultiRate:
		colMethod = "Multi-rate";
		break;
	}
	std::string collisionMethod = "Collision mode :" + colMethod;

	/*
	stringRGBA(m_renderer, m_width - 220, 20, textPhysicsFrames.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, m_width - 220, 40, textTimeFromStart.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, m_width - 220, 60, refreshRate.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, m_width - 220, 80, simulationMode.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, m_width - 220, 100, collisionMethod.c_str(), 255, 255, 255, 255);
	*/

	std::string textPhysicsTime = "Physics Step (ms): " + std::format("{:.4f}", game.GetPhysicsStepTimeMs());


	stringRGBA(m_renderer, 0, 20, textPhysicsFrames.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer,0, 40, textTimeFromStart.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer,0, 60, refreshRate.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 80, simulationMode.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 100, collisionMethod.c_str(), 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 120, "Hotkeys: ", 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 140, "R (reload)", 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 160, "P (pause)", 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 180, "N (next step when paused)", 255, 255, 255, 255);
	stringRGBA(m_renderer,0, 200, "M (next demo)", 255, 255, 255, 255);
	stringRGBA(m_renderer, 0, 220, textPhysicsTime.c_str(), 255, 255, 255, 255);

	SDL_RenderPresent(m_renderer);
}
