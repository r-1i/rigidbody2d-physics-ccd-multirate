#include "Simulation.h"
#include <algorithm>

Simulation::Simulation(std::vector<std::shared_ptr<PhysObject>>* pObjects) : pObjects(pObjects)
{
	for (const auto& pObj : *pObjects) {
		switch (pObj->bodyType)
		{
		case BodyType::Static:
			staticObjects.push_back(pObj);
			break;

		case BodyType::Dynamic:
			dynamicObjects.push_back(pObj);
			break;
		}
	}
}

void Simulation::Update(float dt)
{
	ResolveCollisions();

	for (auto& physObj : *pObjects) {
		if (physObj->bodyType == BodyType::Static) continue;

		physObj->vel += gravity * dt;
		physObj->pos += physObj->vel * dt;
	}
}

void Simulation::ResolveCollisions()
{
	// Dynamic / Static
	for (auto dyn : dynamicObjects) {
		for (auto stat : staticObjects) {
			CollisionData cd = CollisionXY(*dyn, *stat);
			if (cd.isColliding) {
				dyn->pos += cd.normal * cd.penetration;

				if (Vec2::Dot(dyn->vel, cd.normal) < 0) {
					dyn->vel = Vec2::Reflect(dyn->vel, cd.normal);
				}
			}
		}
	}

	// Dynamic / Dynamic
	for (size_t i = 0; i < dynamicObjects.size(); ++i) {
		for (size_t j = i + 1; j < dynamicObjects.size(); ++j) {
			auto dyn1 = dynamicObjects[i];
			auto dyn2 = dynamicObjects[j];

			CollisionData cd = CollisionXY(*dyn1, *dyn2);
			if (cd.isColliding) {
				dyn1->pos += cd.normal * (cd.penetration * 0.5f);
				dyn2->pos -= cd.normal * (cd.penetration * 0.5f);

				Vec2 relativeVel = dyn2->vel - dyn1->vel;

				float velAlongNormal = Vec2::Dot(relativeVel, cd.normal);

				if (velAlongNormal < 0) {
					float e = 1.0f;

					float j = -(1.0f + e) * velAlongNormal / ((1.0f/dyn1->mass) + (1.0f / dyn2->mass));

					Vec2 impulse = cd.normal * j;

					dyn1->vel -= impulse / dyn1->mass;
					dyn2->vel += impulse / dyn2->mass;
				}
			}
		}
	}
}

void Simulation::InitializeGroups()
{
	staticObjects.clear();
	dynamicObjects.clear();

	for (const auto& pObj : *pObjects) {
		if (pObj->bodyType == BodyType::Static)
			staticObjects.push_back(pObj);
		else if (pObj->bodyType == BodyType::Dynamic)
			dynamicObjects.push_back(pObj);
	}
}

CollisionData Simulation::CollisionXY(const PhysObject& a, const PhysObject& b)
{
	if (a.bodyType == BodyType::Static && b.bodyType == BodyType::Static) return CollisionData();
	else if (a.GetShapeType() == ShapeType::Circle && b.GetShapeType() == ShapeType::Circle) return CollisionCircleCircle(a, b);
	else if (a.GetShapeType() == ShapeType::Circle && b.GetShapeType() == ShapeType::AABB) return CollisionCircleAABB(a, b);
	return CollisionData();
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

