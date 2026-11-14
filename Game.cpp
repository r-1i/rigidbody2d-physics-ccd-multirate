#include "Game.h"
#include "PhysObject.h"


Game::Game() : window(Window(800, 600, &objects)), simulation(Simulation(&objects))
{
    for (int i = 0; i < 2; ++i) {
        std::shared_ptr<PhysObject> pCircle = std::make_shared<PhysObject>(
            PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2((i+1)*75.0f, 50), Vec2(0.0f, 400.0f), 50.0f, Vec2(0,0)));
        objects.push_back(pCircle);
    }
}

void Game::Run()
{
    freq = SDL_GetPerformanceFrequency();
    prevCounter = SDL_GetPerformanceCounter();
	
    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                running = false;
        }

        // Update
        simulation.Update(GetDeltaTime());

        // Render
        window.Render();
    }
}

float Game::GetDeltaTime()
{
    Uint64 now = SDL_GetPerformanceCounter();
    float dt = static_cast<float>(now - prevCounter) / static_cast<float>(freq);
    prevCounter = now;
    return dt;
}
