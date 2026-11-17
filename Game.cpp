#include "Game.h"
#include "PhysObject.h"


Game::Game() : window(Window(800, 600, &objects, *this)), simulation(Simulation(&objects))
{
    InitSceneDiscrete();

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
                if (e.key.keysym.sym == SDLK_m) {
                    int nextState = (static_cast<int>(currentState) + 1) % static_cast<int>(SceneState::Count);
                    currentState = static_cast<SceneState>(nextState);

                    LoadScene(currentState);
                }
                if (e.key.keysym.sym == SDLK_r) {
                    LoadScene(currentState);
                }
            }
        }

        float dt = GetDeltaTime();

        if (!isPaused) {
            timeFromStart += dt;
            accumulator += dt;

            while (accumulator >= FIXED_DT) {
                simulation.Update(FIXED_DT, collisionMethod, GetPhysicsFrameCount());
                physicsFrameCount++;
                accumulator -= FIXED_DT;
            }
        } 
        else if (stepNextFrame) {
            simulation.Update(FIXED_DT, collisionMethod, GetPhysicsFrameCount());
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

void Game::LoadScene(SceneState state) {
    objects.clear();

    physicsFrameCount = 0;
    timeFromStart = 0.0f;

    switch (state) {
    case SceneState::DiscreteDemo:
        collisionMethod = CollisionMethod::Discrete;
        InitSceneDiscrete(); 
        break;
    case SceneState::CCDScene:
        collisionMethod = CollisionMethod::CCD;
        InitSceneCCD();        
        break;
    case SceneState::MultiRate1:
        collisionMethod = CollisionMethod::MultiRate;
        InitSceneMultiRate();
        break;
    case SceneState::MultiRate2:
        collisionMethod = CollisionMethod::MultiRate;
        //InitSceneMultiRate2();
        break;
    default:
        break;
    }

    simulation.InitializeGroups();
}

float Game::GetDeltaTime()
{
    Uint64 now = SDL_GetPerformanceCounter();
    float dt = static_cast<float>(now - prevCounter) / static_cast<float>(freq);
    prevCounter = now;
    return dt;
}

void Game::InitSceneDiscrete()
{
    currentRefreshRate = REFRESH_RATE_60;
    std::shared_ptr<PhysObject> pCircle = std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(125.0f, 250.0f), Vec2(2000.0f, -400.0f), 5.0f, Vec2(0, 0)));
    pCircle->mass = 10.0f;
    objects.push_back(pCircle);

    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(705, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(5, 300))));
}

void Game::InitSceneCCD()
{
    currentRefreshRate = REFRESH_RATE_60;
    std::shared_ptr<PhysObject> pCircle = std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(125.0f, 250.0f), Vec2(2000.0f, -400.0f), 5.0f, Vec2(0, 0)));
    pCircle->mass = 10.0f;
    objects.push_back(pCircle);

    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(705, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(5, 300))));

    /*
    // Circles
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(400, 300), Vec2(0.0f, 0.0f), 20.0f, Vec2(0,0))));
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(300, 300), Vec2(0.0f, 0.0f), 20.0f, Vec2(0, 0))));
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(500, 300), Vec2(0.0f, 0.0f), 20.0f, Vec2(0, 0))));
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(600, 300), Vec2(-200.0f, 0.0f), 20.0f, Vec2(0, 0))));

    // Ground
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(400, 600), Vec2(0.0f, 0.0f), 0.0f, Vec2(800, 100))));

    // Walls
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(0, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(100, 300))));
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(800, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(100, 300))));
    */
}

void Game::InitSceneMultiRate()
{
    currentRefreshRate = REFRESH_RATE_60;

    std::shared_ptr<PhysObject> pCircle = std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(125.0f, 250.0f), Vec2(2000.0f, -400.0f), 5.0f, Vec2(0, 0), false));
    pCircle->mass = 10.0f;
    objects.push_back(pCircle);

    std::shared_ptr<PhysObject> pfastCircle = std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(125.0f, 350.0f), Vec2(2000.0f, -400.0f), 5.0f, Vec2(0, 0), true));
    pfastCircle->mass = 10.0f;
    objects.push_back(pfastCircle);

    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(705, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(5, 300))));

    /*
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(50.0f, 300.0f), Vec2(2400.0f, 0.0f), 5.0f, Vec2(0, 0), true)));

    // Ordinary circles
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(400, 100), Vec2(0.0f, 0.0f), 20.0f, Vec2(0, 0))));
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::Circle, BodyType::Dynamic, Vec2(500, 100), Vec2(-50.0f, 0.0f), 20.0f, Vec2(0, 0))));

    // Thin wall — 20 px thick; bullet проходит 10 px за сабстеп 1/240
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(420, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(10, 100))));

    // Ground
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(400, 600), Vec2(0.0f, 0.0f), 0.0f, Vec2(800, 100))));

    // Walls
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(0, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(100, 300))));
    objects.push_back(std::make_shared<PhysObject>(
        PhysObject(ShapeType::AABB, BodyType::Static, Vec2(800, 300), Vec2(0.0f, 0.0f), 0.0f, Vec2(100, 300))));
        */
}
