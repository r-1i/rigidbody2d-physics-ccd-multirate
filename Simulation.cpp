#include "Simulation.h"
#include <algorithm>

Simulation::Simulation(std::vector<std::shared_ptr<PhysObject>>* pObjects) : pObjects(pObjects)
{
	InitializeGroups();
}

void Simulation::Update(float dt, CollisionMethod method, int physFrameCount)
{
	switch (method)
	{
	case CollisionMethod::Discrete:
		UpdateDiscrete(dt);
		break;
	case CollisionMethod::CCD:
		UpdateCCD(dt);
		break;

	case CollisionMethod::MultiRate:
		UpdateMultiRate(dt);
		break;
	}
}

void Simulation::ResolveCollisionsDiscrete()
{
	// Ordinary / Static
	for (auto dyn : ordinaryObjects) {
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

	// Ordinary / Ordinary
	for (size_t i = 0; i < ordinaryObjects.size(); ++i) {
		for (size_t j = i + 1; j < ordinaryObjects.size(); ++j) {
			ResolveDynamicPair(ordinaryObjects[i], ordinaryObjects[j]);
		}
	}
}

void Simulation::ResolveDynamicPair(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b)
{
	CollisionData cd = CollisionXY(*a, *b);
	if (!cd.isColliding) return;

	a->pos -= cd.normal * (cd.penetration * 0.5f);
	b->pos += cd.normal * (cd.penetration * 0.5f);

	Vec2 relativeVel = b->vel - a->vel;

	float velAlongNormal = Vec2::Dot(relativeVel, cd.normal);

	if (velAlongNormal < 0) {
		float e = std::max(a->e, b->e);

		float j = -(1.0f + e) * velAlongNormal / ((1.0f / a->mass) + (1.0f / b->mass));

		Vec2 impulse = cd.normal * j;

		a->vel -= impulse / a->mass;
		b->vel += impulse / b->mass;
	}
}

void Simulation::ResolveFastCollisions()
{
	// Fast / Static
	for (auto& fast : fastObjects) {
		for (auto& stat : staticObjects) {
			CollisionData cd = CollisionXY(*fast, *stat);
			if (cd.isColliding) {
				fast->pos += cd.normal * cd.penetration;

				if (Vec2::Dot(fast->vel, cd.normal) < 0) {
					fast->vel = Vec2::Reflect(fast->vel, cd.normal);
				}
			}
		}
	}

	// Fast / Ordinary
	for (auto& fast : fastObjects) {
		for (auto& ord : ordinaryObjects) {
			ResolveDynamicPair(fast, ord);
		}
	}

	// Fast / Fast
	for (size_t i = 0; i < fastObjects.size(); ++i) {
		for (size_t j = i + 1; j < fastObjects.size(); ++j) {
			ResolveDynamicPair(fastObjects[i], fastObjects[j]);
		}
	}
}

void Simulation::UpdateCCD(float dt)
{
	float timeRemaining = dt;
	int maxIterations = 15; 

	while (timeRemaining > 0 && maxIterations > 0) {
		maxIterations--;

		float min_tc = timeRemaining + 1.0f;
		std::shared_ptr<PhysObject> hitA = nullptr;
		CollisionDataCCD best_cd;

		for (auto& dyn : ordinaryObjects) {
			CollisionDataCCD cd = FindClosestCollisionCCD(dyn, timeRemaining);
			if (cd.tc >= 0.0f && cd.tc < min_tc) {
				min_tc = cd.tc;
				hitA = dyn;
				best_cd = cd;
			}
		}
		for (auto& dyn : fastObjects) {
			CollisionDataCCD cd = FindClosestCollisionCCD(dyn, timeRemaining);
			if (cd.tc >= 0.0f && cd.tc < min_tc) {
				min_tc = cd.tc;
				hitA = dyn;
				best_cd = cd;
			}
		}

		if (min_tc >= 0.0f && min_tc <= timeRemaining) {
			for (auto& dyn : ordinaryObjects) {
				dyn->pos += dyn->vel * min_tc;
				dyn->vel += gravity * min_tc;
			}
			for (auto& dyn : fastObjects) {
				dyn->pos += dyn->vel * min_tc;
				dyn->vel += gravity * min_tc;
			}

			if (best_cd.hitObject) {
				if (best_cd.hitObject->bodyType == BodyType::Static) {
					if (Vec2::Dot(hitA->vel, best_cd.normal) < 0) {
						float e = std::max(hitA->e, best_cd.hitObject->e);
						hitA->vel = Vec2::Reflect(hitA->vel, best_cd.normal) * e;
					}
				}
				else if (best_cd.hitObject->bodyType == BodyType::Dynamic) {
					auto hitB = best_cd.hitObject;
					Vec2 relativeVel = hitA->vel - hitB->vel;
					float velAlongNormal = Vec2::Dot(relativeVel, best_cd.normal);

					if (velAlongNormal < 0) {
						float e = std::max(hitA->e, hitB->e);
						float j = -(1.0f + e) * velAlongNormal / ((1.0f / hitA->mass) + (1.0f / hitB->mass));
						Vec2 impulse = best_cd.normal * j;

						hitA->vel += impulse / hitA->mass;
						hitB->vel -= impulse / hitB->mass;
					}
				}
			}

			const float EPSILON = 0.01f;
			hitA->pos += best_cd.normal * EPSILON;
			if (best_cd.hitObject && best_cd.hitObject->bodyType == BodyType::Dynamic) {
				best_cd.hitObject->pos -= best_cd.normal * EPSILON;
			}

			timeRemaining -= std::max(min_tc, 0.0001f);
		}
		else {
			for (auto& dyn : ordinaryObjects) {
				dyn->pos += dyn->vel * timeRemaining;
				dyn->vel += gravity * timeRemaining;
			}
			for (auto& dyn : fastObjects) {
				dyn->pos += dyn->vel * timeRemaining;
				dyn->vel += gravity * timeRemaining;
			}
			timeRemaining = 0;
		}
	}
}

CollisionDataCCD Simulation::FindClosestCollisionCCD(const std::shared_ptr<PhysObject>& physObject, float timeRemaining)
{
	CollisionDataCCD closestCollision;

	closestCollision.tc = timeRemaining + 1.0f;

	for (const auto& statObj : staticObjects) {
		CollisionDataCCD cd = CollisionXY_CCD(physObject, statObj);

		if (cd.tc >= 0.0f && cd.tc < closestCollision.tc) {
			closestCollision = cd;
			closestCollision.hitObject = statObj;
		}
	}

	for (const auto& dynObj : ordinaryObjects) {
		if (physObject == dynObj) continue;

		CollisionDataCCD cd = CollisionXY_CCD(physObject, dynObj);

		if (cd.tc >= 0.0f && cd.tc < closestCollision.tc) {
			closestCollision = cd;
			closestCollision.hitObject = dynObj;
		}
	}

	for (const auto& dynObj : fastObjects) {
		if (physObject == dynObj) continue;

		CollisionDataCCD cd = CollisionXY_CCD(physObject, dynObj);

		if (cd.tc >= 0.0f && cd.tc < closestCollision.tc) {
			closestCollision = cd;
			closestCollision.hitObject = dynObj;
		}
	}

	return closestCollision;
}

CollisionDataCCD Simulation::CollisionXY_CCD(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b)
{
	if (a->GetShapeType() == ShapeType::Circle && b->GetShapeType() == ShapeType::Circle) { return CollisionCircleCircle_CCD(a, b); }
	else { return CollisionCircleAABB_CCD(a, b); }
}

CollisionDataCCD Simulation::CollisionCircleCircle_CCD(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b)
{
	CollisionDataCCD cd;
	cd.tc = -1.0f;

	Vec2 P = a->pos - b->pos; // vec between centers
	Vec2 V = a->vel - b->vel; // relative pos

	float R = a->radius + b->radius;

	float a_coeff = Vec2::Dot(V, V); // relative vel squared
	float b_coeff = 2.0f * Vec2::Dot(P, V);
	float c_coeff = Vec2::Dot(P, P) - (R * R);

	if (c_coeff <= 0.0f) {
		cd.tc = 0.0f;
		cd.normal = P.Normalized();
		return cd;
	}

	// if relative vel equals zero, then circles moving in different ways
	if (a_coeff < 1e-8f) {
		return cd;
	}

	float D = b_coeff * b_coeff - 4.0f * a_coeff * c_coeff;
	if (D < 0.0f) { // no roots
		return cd;
	}

	float sqrtD = std::sqrt(D);
	float t1 = (-b_coeff - sqrtD) / (2.0f * a_coeff);
	float t2 = (-b_coeff + sqrtD) / (2.0f * a_coeff);

	float t_c = -1.0f;
	if (t1 >= 0.0f) { t_c = t1; }
	else if (t2 >= 0.0f) { t_c = t2; }

	if (t_c >= 0.0f) {
		cd.tc = t_c;

		Vec2 posA_tc = a->pos + (a->vel * t_c);
		Vec2 posB_tc = b->pos + (b->vel * t_c);

		cd.normal = (posA_tc - posB_tc).Normalized();
	}

	return cd;
}

CollisionDataCCD Simulation::CollisionCircleAABB_CCD(const std::shared_ptr<PhysObject>& a, const std::shared_ptr<PhysObject>& b)
{
	CollisionDataCCD cd;
	cd.tc = -1.0f;

	auto circle = (a->GetShapeType() == ShapeType::Circle) ? a : b;
	auto aabb = (a->GetShapeType() == ShapeType::AABB) ? a : b;

	float r = circle->radius;
	float minX = aabb->pos.x - aabb->halfExtents.x - r;
	float maxX = aabb->pos.x + aabb->halfExtents.x + r;
	float minY = aabb->pos.y - aabb->halfExtents.y - r;
	float maxY = aabb->pos.y + aabb->halfExtents.y + r;

	Vec2 p = circle->pos;

	// initial penetration: circle center is already inside the expanded AABB
	if (p.x > minX && p.x < maxX && p.y > minY && p.y < maxY) {
		float left = p.x - minX;
		float right = maxX - p.x;
		float top = p.y - minY;
		float bottom = maxY - p.y;

		Vec2 normal(0, 1);
		float minPen = bottom;
		if (top < minPen)    { minPen = top;    normal = Vec2(0, -1); }
		if (left < minPen)   { minPen = left;   normal = Vec2(-1, 0); }
		if (right < minPen)  { minPen = right;  normal = Vec2(1, 0); }

		cd.tc = 0.0f;
		cd.normal = (circle == b) ? -normal : normal;
		return cd;
	}

	Vec2 v = circle->vel - aabb->vel;

	if (v.LengthSq() < 1e-8f) {
		return cd;
	}

	float txEntry, txExit;
	Vec2 nx; 

	if (std::abs(v.x) < 1e-8f) {
		if (p.x < minX || p.x > maxX) return cd;
		txEntry = -1e9f;
		txExit = 1e9f;
	}
	else {
		float tx1 = (minX - p.x) / v.x;
		float tx2 = (maxX - p.x) / v.x;

		if (tx1 < tx2) {
			txEntry = tx1; txExit = tx2; nx = Vec2(-1, 0);
		}
		else {
			txEntry = tx2; txExit = tx1; nx = Vec2(1, 0);
		}
	}

	float tyEntry, tyExit;
	Vec2 ny;

	if (std::abs(v.y) < 1e-8f) {
		if (p.y < minY || p.y > maxY) return cd;
		tyEntry = -1e9f;
		tyExit = 1e9f;
	}
	else {
		float ty1 = (minY - p.y) / v.y;
		float ty2 = (maxY - p.y) / v.y;

		if (ty1 < ty2) {
			tyEntry = ty1; 
			tyExit = ty2; 
			ny = Vec2(0, -1); 
		}
		else {
			tyEntry = ty2; 
			tyExit = ty1; 
			ny = Vec2(0, 1); 
		}
	}

	float tIn = std::max(txEntry, tyEntry);
	float tOut = std::min(txExit, tyExit);

	if (tIn > tOut || tOut < 0.0f) {
		return cd;
	}

	if (tIn >= 0.0f) {
		cd.tc = tIn;
		Vec2 normal = (txEntry > tyEntry) ? nx : ny;

		if (circle == b) {
			cd.normal = -normal;
		}
		else {
			cd.normal = normal;
		}
	}

	return cd;
}

void Simulation::UpdateDiscrete(float dt)
{
	ResolveCollisionsDiscrete();
	ResolveFastCollisions();

	for (auto& physObj : *pObjects) {
		if (physObj->bodyType == BodyType::Static) continue;

		physObj->vel += gravity * dt;
		physObj->pos += physObj->vel * dt;
	}
}

void Simulation::InitializeGroups()
{
	staticObjects.clear();
	ordinaryObjects.clear();
	fastObjects.clear();

	for (const auto& pObj : *pObjects) {
		if (pObj->bodyType == BodyType::Static) {
			staticObjects.push_back(pObj);
		}
		else if (pObj->bodyType == BodyType::Dynamic) {
			if (pObj->isFast)
				fastObjects.push_back(pObj);
			else
				ordinaryObjects.push_back(pObj);
		}
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

void Simulation::UpdateMultiRate(float dt)
{
	const int subSteps = 4;
	const float sub_dt = dt / static_cast<float>(subSteps);

	ResolveCollisionsDiscrete();
	for (auto& obj : ordinaryObjects) {
		obj->vel += gravity * dt;
		obj->pos += obj->vel * dt;
	}

	for (int i = 0; i < subSteps; ++i) {
		ResolveFastCollisions();

		for (auto& obj : fastObjects) {
			obj->vel += gravity * sub_dt;
			obj->pos += obj->vel * sub_dt;
		}
	}
}
