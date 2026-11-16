#pragma once

#include <vector>
#include "PhysObject.h"
#include <memory>
#include "CollisionData.h"

class Simulation {
public:
	Simulation(std::vector<std::shared_ptr<PhysObject>>* pObjects);
	void Update(float dt);
	void InitializeGroups();

private:
	void ResolveCollisionsDiscrete();
	CollisionDataCCD FindClosestCollisionCCD(const std::shared_ptr<PhysObject>& physObject, float timeRemaining);

	CollisionDataCCD CollisionXY_CCD(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b);
	CollisionDataCCD CollisionCircleCircle_CCD(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b);
	CollisionDataCCD CollisionCircleAABB_CCD(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b);

	void UpdateDiscrete(float dt);
	void UpdateCCD(float dt);


	CollisionData CollisionXY(const PhysObject& a, const PhysObject& b);
	CollisionData CollisionCircleCircle(const PhysObject& a, const PhysObject& b);
	CollisionData CollisionCircleAABB(const PhysObject& a, const PhysObject& b);
	CollisionData CollisionAABBAABB(const PhysObject& a, const PhysObject& b);

	std::vector<std::shared_ptr<PhysObject>>* pObjects;
	std::vector<std::shared_ptr<PhysObject>> staticObjects;
	std::vector<std::shared_ptr<PhysObject>> dynamicObjects;

	Vec2 gravity = Vec2(0.0f, 980.0f);
};