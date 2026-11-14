#include "Simulation.h"

Simulation::Simulation(std::vector<std::shared_ptr<PhysObject>>* pObjects) : pObjects(pObjects)
{

}

void Simulation::Update(float dt)
{
	for (auto& physObj : *pObjects) {
		if (physObj->bodyType == BodyType::Static) continue;
		physObj->vel = physObj->vel;
		physObj->pos += physObj->vel * dt;
	}
}

