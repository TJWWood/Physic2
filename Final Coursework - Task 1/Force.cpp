#include <iostream>
#include <cmath>
#include "Force.h"
#include "Body.h"
#include "glm/ext.hpp"

glm::vec3 Force::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	return glm::vec3(0.0f);
}

glm::vec3 Gravity::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	return getGravity() * mass;
}

//glm::vec3 Drag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
//{

//}

glm::vec3 Hooke::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	float ks = getKs();
	float kd = getKd();
	float rest = getRest();
	Body* b1 = getB1();
	Body* b2 = getB2();

	float l = glm::length(b2->getPos() - b1->getPos());
	glm::vec3 e = normalize(b2->getPos() - b1->getPos());
	float v1 = dot(b1->getVel(),e);
	float v2 = dot(b2->getVel(),e);

	float fs = -ks * (rest - l);
	float fd = kd * (v1 - v2);
	float hooke = fs - fd;

	return hooke * e;
}