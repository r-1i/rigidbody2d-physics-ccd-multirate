#include "Game.h"
#include "PhysObject.h"


Game::Game() : window(Window(800, 600, &objects, *this)), simulation(Simulation(&objects))
{
    currentRefreshRate = REFRESH_RATE_240;
    std::shared_ptr<PhysObject> pCircle = std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(125.0f, 250.0f), Vec2(2000.0f, -400.0f), 5.0f, Vec2(0, 0)));
    pCircle->mass = 10.0f;
    objects.push_back(pCircle);

    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(705, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(5, 300))));

    simulation.InitializeGroups();
}

void Game::Run()
{
    freq = SDL_GetPerformanceFrequency();
    prevCounter = SDL_GetPerformanceCounter();
	
    bool running = true;
    bool isPaused = false;
    bool stepNextFrame = false;

    const float FIXED_DT = 1.0f / currentRefreshRate;
    float accumulator = 0.0f;

    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; }
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {

                if (e.key.keysym.sym == SDLK_ESCAPE) {running = false;}
                if (e.key.keysym.sym == SDLK_p) {isPaused = !isPaused;}
                if (e.key.keysym.sym == SDLK_n && isPaused) {stepNextFrame = true;}
            }
        }

        float dt = GetDeltaTime();

        if (!isPaused) {
            timeFromStart += dt;
            accumulator += dt;

            while (accumulator >= FIXED_DT) {
                simulation.Update(FIXED_DT);
                physicsFrameCount++;
                accumulator -= FIXED_DT;
            }
        } 
        else if (stepNextFrame) {
            simulation.Update(FIXED_DT);
            timeFromStart += FIXED_DT;
            physicsFrameCount++;
            stepNextFrame = false;
        }

        // Render
        window.Render();
    }
}

int Game::GetPhysicsFrameCount() const
{
    return physicsFrameCount;
}

float Game::GetTime() const
{
    return timeFromStart;
}

float Game::GetDeltaTime()
{
    Uint64 now = SDL_GetPerformanceCounter();
    float dt = static_cast<float>(now - prevCounter) / static_cast<float>(freq);
    prevCounter = now;
    return dt;
}
