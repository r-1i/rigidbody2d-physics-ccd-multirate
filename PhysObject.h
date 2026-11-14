#pragma once

#include "Vec2.h"

enum class ShapeType { Circle, AABB };
enum class BodyType { Static, Dynamic };

class PhysObject {
public:
	PhysObject(ShapeType shapeType, BodyType bodyType, Vec2 pos, Vec2 vel, float radius, Vec2 halfExtents) 
		: shapeType(shapeType), bodyType(bodyType), pos(pos), vel(vel), radius(radius), halfExtents(halfExtents) {};
	
	Vec2 pos;
	Vec2 vel;
	
	BodyType bodyType;
	
	float radius;
	Vec2 halfExtents;

	ShapeType GetShapeType() const { return shapeType; }

private:
	ShapeType shapeType;

};