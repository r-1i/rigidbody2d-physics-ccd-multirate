#pragma once

#include <vector>
#include "PhysObject.h"
#include <memory>
#include "CollisionData.h"

class Simulation {
public:
	Simulation(std::vector<std::shared_ptr<PhysObject>>* pObjects);
	void Update(float dt);

private:
	std::vector<std::shared_ptr<PhysObject>>* pObjects;
	CollisionData CollisionXY(const PhysObject& a, const PhysObject& b);
	CollisionData CollisionCircleCircle(const PhysObject& a, const PhysObject& b);
	CollisionData CollisionCircleAABB(const PhysObject& a, const PhysObject& b);
	CollisionData CollisionAABBAABB(const PhysObject& a, const PhysObject& b);
};