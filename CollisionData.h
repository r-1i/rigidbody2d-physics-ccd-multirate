#pragma once

#include "Vec2.h"

struct CollisionData {
	bool isColliding = false;
	Vec2 normal = Vec2(0.0f, 0.0f);
	float penetration = 0.0f;
};