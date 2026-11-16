#pragma once

#include "Vec2.h"
class PhysObject;

struct CollisionData {
	bool isColliding = false;
	Vec2 normal = Vec2(0.0f, 0.0f);
	float penetration = 0.0f;
};

struct CollisionDataCCD {
	CollisionDataCCD() = default;
	float tc = 0.0f;
	Vec2 normal = Vec2(0.0f, 0.0f);
	std::shared_ptr<PhysObject> hitObject = nullptr;
};