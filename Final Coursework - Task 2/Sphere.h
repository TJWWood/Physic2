#pragma once
#include "RigidBody.h"
#include "glm/ext.hpp"

class Sphere :
	public RigidBody
{
public:
	Sphere();
	~Sphere();

	void setCenter() { center = getPos(); }
	void setRadius(float r) { radius = r; }

	glm::vec3 getCenter() { return center; }
	float getRadius() { return radius; }


private:
	glm::vec3 center;
	float radius;
};
