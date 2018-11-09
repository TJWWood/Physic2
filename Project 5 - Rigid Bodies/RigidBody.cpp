#include "RigidBody.h"

RigidBody::RigidBody()
{

}

RigidBody::~RigidBody()
{

}

void RigidBody::calcIntertia() {
	double m = m_mass;
	double x = m_scale.x;
	double y = m_scale.y;
	double z = m_scale.z;

	std::cout <<"SCALE:" << x << y << z;
	
	glm::mat3 I0;
	I0[0][0] = pow(12, -1) * m * ((y * y) + (z * z));
	I0[1][1] = pow(12, -1) * m * (x * x + z * z);
	I0[2][2] = pow(12, -1) * m * (x * x + y * y);
	
	m_invInertia = getRotate() * glm::inverse(I0) * glm::transpose(getRotate());
	//std::cout << glm::to_string(m_invInertia);
	//setInvInertia(-I0);
}