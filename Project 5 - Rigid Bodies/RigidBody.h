#pragma once
#include "Body.h"

class RigidBody :
	public Body
{
public:
	RigidBody();
	~RigidBody();

	void setAngVel(const glm::vec3 &omega) { m_angVel = omega; }
	void setAngAccl(const glm::vec3 &alpha) { m_angAcc = alpha; }
	void setInvInertia(const glm::mat3 &invInertia) { m_invInertia = invInertia; }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngAcc() { return m_angAcc; }
	glm::mat3 getInvInertia() { return m_invInertia; }

	void scale(glm::vec3 vect);


	//TODO STUFF HERE
	glm::mat3 RigidBody::calcIntertia(float m, float x, float y, float z) const;


private:
	float m_density;
	glm::mat3 m_invInertia;
	glm::vec3 m_angVel;
	glm::vec3 m_angAcc;
};

