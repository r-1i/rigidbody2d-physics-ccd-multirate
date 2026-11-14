#include "Game.h"
#include "PhysObject.h"


Game::Game() : window(Window(800, 600))
{

}

void Game::Run()
{
	PhysObject circle(ShapeType::Circle, BodyType::Static, Vec2(50, 50), Vec2(0, 0), 50.0f, Vec2(0, 0));
	PhysObject rectangle(ShapeType::AABB, BodyType::Static, Vec2(150, 150), Vec2(0, 0), 0.0f, Vec2(50, 50));
	
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

        // Render
        window.AddToBuffer(&circle);
        window.AddToBuffer(&rectangle);

        window.Render();

        window.ClearBuffer();
    }

}
