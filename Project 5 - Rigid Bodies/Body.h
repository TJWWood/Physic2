#pragma once
#include "Mesh.h"
#include "Force.h"

class Body {

private:
	Mesh m_mesh;
	float m_mass;
	float m_cor;

	glm::vec3 m_acc;
	glm::vec3 m_vel;
	glm::vec3 m_pos;

	std::vector<Force*> m_forces;

public:

	Mesh& getMesh() { return m_mesh; }

	glm::mat3 getTranslate() const { return m_mesh.getTranslate(); }
	glm::mat3 getRotate() const { return m_mesh.getRotate(); }
	glm::mat3 getScale() const { return m_mesh.getScale(); }
	std::vector<Force*> getForces() { return m_forces; }

	glm::vec3& getAcc() {
		return m_acc;
	}

	glm::vec3& getVel() {
		return m_vel;
	}

	glm::vec3& getPos() {
		return m_pos;
	}

	float getMass() const {
		return m_mass;
	}

	float getCor() {
		return m_cor;
	}

	void setMesh(Mesh m) { m_mesh = m; }

	// dynamic variables
	void setAcc(const glm::vec3 & vect) { m_acc = vect; }
	void setVel(const glm::vec3 & vect) { m_vel = vect; }
	void setVel(int i, float v) { m_vel[i] = v; } // set the ith coordinate of the velocity vector
	void setPos(const glm::vec3 & vect) { m_pos = vect; m_mesh.setPos(vect); }
	void setPos(int i, float p) { m_pos[i] = p; m_mesh.setPos(i, p); } // set the ith coordinate of the position vector
	
	void setRotate(const glm::mat4 &mat) { m_mesh.setRotate(mat); }

	glm::vec3 Body::applyForces(glm::vec3 x, glm::vec3 v, float t, float dt) {
		glm::vec3 fAccumulator = glm::vec3(0.0f);

		for (auto &f : m_forces)
		{
			fAccumulator += f->apply(getMass(), x, v);
		}
		return fAccumulator / getMass();
	}

	// physical properties
	void setCor(float cor) { m_cor = cor; }
	void virtual setMass(float mass) { m_mass = mass; }


	Body();

	~Body();

	void translate(const glm::vec3 &vect);
	void rotate(float angle, const glm::vec3 &vect);
	void virtual scale(const glm::vec3 &vect);
	void addForce(Force *f) { m_forces.push_back(f); }

};
