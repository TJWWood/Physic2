#include "RigidBody.h"

RigidBody::RigidBody()
{

}

RigidBody::~RigidBody()
{

}

void RigidBody::calcIntertia() {
	float m = getMass();
	float x = getScale()[0][0] * 2.0f;
	float y = getScale()[1][1] * 2.0f;
	float z = getScale()[2][2] * 2.0f;

	//std::cout <<"SCALE:" << x << y << z;
	
	glm::mat3 I0 = glm::mat3(0.0f);
	I0[0][0] = (1.0f / 12.0f) * m * ((y * y) + (z * z));
	I0[1][1] = (1.0f / 12.0f) * m * ((x * x) + (z * z));
	I0[2][2] = (1.0f / 12.0f) * m * ((x * x) + (y * y));
	
	setInvInertia(getRotate() * glm::inverse(I0) * glm::transpose(getRotate()));
	//std::cout << glm::to_string(getInvInertia());
	std::cout << glm::to_string(m_invInertia);
	//setInvInertia(-I0);
}