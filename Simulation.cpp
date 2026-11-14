#include "Simulation.h"
#include <algorithm>

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

CollisionData Simulation::CollisionXY(const PhysObject& a, const PhysObject& b)
{
	// As / As -> nothing
	// Cs / Cs -> nothing
	// Cs / As -> nothing

	///////////////////////
	// Cs / Cd -> Cd moving
	// Cd / Cs -> Cd moving
	// Cd / Cd -> both Cd moving
	// 
	// Cs / Ad -> Ad moving
	// Cd / As -> Cd moving
	// Cd / Ad -> both Ad Cd moving
	// 
	// As / Ad -> Cd moving
	// Ad / As -> Cd moving
	// Ad / Ad -> both Cd moving

	if (a.bodyType == BodyType::Static && b.bodyType == BodyType::Static) return CollisionData();
}

CollisionData Simulation::CollisionCircleCircle(const PhysObject& a, const PhysObject& b)
{
	float radiusSum = (a.radius + b.radius);
	float sumRadiusSq = radiusSum * radiusSum;
	float distanceSq = Vec2::DistanceSq(a.pos, b.pos);
	if (sumRadiusSq > distanceSq) {
		CollisionData cd;
		cd.isColliding = true;
		cd.normal = (b.pos - a.pos).Normalized();
		cd.penetration = radiusSum - Vec2::Distance(a.pos, b.pos);
		return cd;
	}
	return CollisionData();
}

CollisionData Simulation::CollisionCircleAABB(const PhysObject& a, const PhysObject& b)
{
	const PhysObject& circle = a.GetShapeType() == ShapeType::Circle ? a : b;
	const PhysObject& aabb = a.GetShapeType() == ShapeType::AABB ? a : b;

	float closestX = std::clamp(circle.pos.x, aabb.pos.x - aabb.halfExtents.x, aabb.pos.x + aabb.halfExtents.x);
	float closestY = std::clamp(circle.pos.y, aabb.pos.y - aabb.halfExtents.y, aabb.pos.y + aabb.halfExtents.y);

	float dx = circle.pos.x - closestX;
	float dy = circle.pos.y - closestY;

	float distSq = dx * dx + dy * dy;

	if (distSq < circle.radius * circle.radius) {
		CollisionData cd;
		cd.isColliding = true;

		if (distSq > 1e-8f) {
			float dist = std::sqrt(distSq);
			cd.normal = Vec2(dx, dy) / dist;
			cd.penetration = circle.radius - dist;
		}
		else {
			float left = circle.pos.x - (aabb.pos.x - aabb.halfExtents.x);
			float right = (aabb.pos.x + aabb.halfExtents.x) - circle.pos.x;
			float bottom = circle.pos.y - (aabb.pos.y - aabb.halfExtents.y);
			float top = (aabb.pos.y + aabb.halfExtents.y) - circle.pos.y;
			float minPen = std::min({ left, right, bottom, top });
			if (minPen == left)       cd.normal = Vec2(-1, 0);
			else if (minPen == right) cd.normal = Vec2(1, 0);
			else if (minPen == bottom)cd.normal = Vec2(0, -1);
			else                      cd.normal = Vec2(0, 1);
			cd.penetration = circle.radius + minPen;
		}
		return cd;
	}
	return CollisionData();
}

CollisionData Simulation::CollisionAABBAABB(const PhysObject& a, const PhysObject& b)
{
	return CollisionData();
}

