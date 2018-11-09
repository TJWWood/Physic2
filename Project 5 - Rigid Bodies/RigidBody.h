#pragma once
#include "Body.h"
#include "glm/ext.hpp"

class RigidBody :
	public Body
{
public:
	RigidBody();
	~RigidBody();

	void calcIntertia();

	void setAngVel(const glm::vec3 &omega) { m_angVel = omega; }
	void setAngAccl(const glm::vec3 &alpha) { m_angAcc = alpha; }
	void setInvInertia(const glm::mat3 &invInertia) { m_invInertia = invInertia; }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngAcc() { return m_angAcc; }
	glm::mat3 getInvInertia() { return m_invInertia; }

	void RigidBody::scale(glm::vec3 vect)
	{
		__super::scale(vect);
		m_scale = vect;
		std::cout << "Set Scale: " << std::endl;
		calcIntertia();
		//std::cout << glm::to_string(m_invInertia);
	}

	void RigidBody::setMass(float mass)
	{
		__super::setMass(mass);
		m_mass = mass;
		std::cout << "Set Mass: " << mass << std::endl;
		calcIntertia();
		std::cout << glm::to_string(m_invInertia);
	}


private:
	float m_density;
	glm::mat3 m_invInertia;
	glm::vec3 m_angVel;
	glm::vec3 m_angAcc;
	float m_mass;
	glm::vec3 m_scale;
};

