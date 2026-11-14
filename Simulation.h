#pragma once

#include <vector>
#include "PhysObject.h"
#include <memory>

class Simulation {
public:
	Simulation(std::vector<std::shared_ptr<PhysObject>>* pObjects);
	void Update(float dt);

private:
	std::vector<std::shared_ptr<PhysObject>>* pObjects;
};