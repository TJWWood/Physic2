#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"

// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Body.h"
#include "Particle.h"
#include "RigidBody.h"
#include "sphere.h"


// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));

	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x5
	plane.scale(glm::vec3(30.0f, 1.0f, 30.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);

	Mesh leftWall = Mesh::Mesh(Mesh::CUBE);
	leftWall.scale(glm::vec3(1.0f, 1.0f, 30.0f));
	leftWall.translate(glm::vec3(-31.0f, 1.0f, 0.0f));
	leftWall.setShader(lambert);

	Mesh rightWall = Mesh::Mesh(Mesh::CUBE);
	rightWall.scale(glm::vec3(1.0f, 1.0f, 30.0f));
	rightWall.translate(glm::vec3(31.0f, 1.0f, 0.0f));
	rightWall.setShader(lambert);

	Mesh topWall = Mesh::Mesh(Mesh::CUBE);
	topWall.scale(glm::vec3(30.0f, 1.0f, 1.0f));
	topWall.translate(glm::vec3(0.0f, 1.0f, 31.0f));
	topWall.setShader(lambert);

	Mesh bottomWall = Mesh::Mesh(Mesh::CUBE);
	bottomWall.scale(glm::vec3(30.0f, 1.0f, 1.0f));
	bottomWall.translate(glm::vec3(0.0f, 1.0f, -31.0f));
	bottomWall.setShader(lambert);
	// time
	GLfloat firstFrame = (GLfloat)glfwGetTime();

	double t = 0.0;
	const double dt = 0.016;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;

	Gravity g = Gravity(glm::vec3(0.0f, -9.8f, 0.0f));

	const int NUM_SPHERES = 1200;

	Mesh m = Mesh::Mesh("resources/models/sphere.obj");

	RigidBody sphere[NUM_SPHERES];
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<float> dist(-200.0f, 200.0f);
	std::uniform_real_distribution<float> moveDir(-5.0f, 5.0f);
	Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	for (int i = 0; i < NUM_SPHERES; i++)
	{
		sphere[i].setMesh(m);
		
		sphere[i].getMesh().setShader(rbShader);
		sphere[i].scale(glm::vec3(1.0f, 1.0f, 1.0f));

		sphere[i].translate(glm::vec3(dist(e2), 1.0f, dist(e2)));
		sphere[i].setVel(glm::vec3(moveDir(e2), 0.0f, moveDir(e2)));
		sphere[i].setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
		sphere[i].setMass(2.0f);
	}

	std::vector<RigidBody*> topLeft;
	std::vector<RigidBody*> topLeftLeft;
	std::vector<RigidBody*> topRight;
	std::vector<RigidBody*> topRightRight;
	std::vector<RigidBody*> bottomLeft;
	std::vector<RigidBody*> bottomLeftLeft;
	std::vector<RigidBody*> bottomRight;
	std::vector<RigidBody*> bottomRightRight;

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		app.showFPS();
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;

		accumulator += frameTime;
		
		while (accumulator >= dt)
		{

			/*
			**	INTERACTION
			*/
			// Manage interaction
			app.doMovement(dt);

			/*
			**	SIMULATION
			*/
			//float e = 1.0f;
			for each(RigidBody &sphere1 in sphere)
			{
				sphere1.translate(sphere1.getVel() * dt);
				if (sphere1.getPos().x - 1.0f <= -1000.0f)
				{
					sphere1.translate(glm::vec3(0.2f, 0.0f, 0.0f));
					sphere1.getVel().x *= -1.0f;
				}
				else if (sphere1.getPos().x + 1.0f >= 1000.0f)
				{
					sphere1.translate(glm::vec3(-0.2f, 0.0f, 0.0f));
					sphere1.getVel().x *= -1.0f;
				}
				else if (sphere1.getPos().z + 1.0f >= 1000.0f)
				{
					sphere1.translate(glm::vec3(0.0f, 0.0f, -0.2f));
					sphere1.getVel().z *= -1.0f;
				}
				else if (sphere1.getPos().z - 1.0f <= -1000.0f)
				{
					sphere1.translate(glm::vec3(0.0f, 0.0f, 0.2f));
					sphere1.getVel().z *= -1.0f;
				}
				
				if (sphere1.getPos().x < 0.0f && sphere1.getPos().x > -500.0f && sphere1.getPos().z < 0.0f)
				{
					topLeft.push_back(&sphere1);
				}
				if (sphere1.getPos().x < -500.0f && sphere1.getPos().z < 0.0f)
				{
					topLeftLeft.push_back(&sphere1);
				}
				if (sphere1.getPos().x < 0.0f && sphere1.getPos().x > -500.0f && sphere1.getPos().z > 0.0f)
				{
					bottomLeft.push_back(&sphere1);
				}
				if (sphere1.getPos().x < -500.0f && sphere1.getPos().z > 0.0f)
				{
					bottomLeftLeft.push_back(&sphere1);
				}
				if (sphere1.getPos().x > 0.0f && sphere1.getPos().x < 500.0f && sphere1.getPos().z < 0.0f)
				{
					topRight.push_back(&sphere1);
				}
				if (sphere1.getPos().x > 500.0f && sphere1.getPos().z < 0.0f)
				{
					topRightRight.push_back(&sphere1);
				}
				if (sphere1.getPos().x > 0.0f && sphere1.getPos().x < 500.0f && sphere1.getPos().z > 0.0f)
				{
					bottomRight.push_back(&sphere1);
				}
				if (sphere1.getPos().x > 500.0f && sphere1.getPos().z > 0.0f)
				{
					bottomRightRight.push_back(&sphere1);
				}

			}

			for each(RigidBody* l in topLeft)
			{
				for each (RigidBody* r in topLeft)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}
			for each(RigidBody* l in topLeftLeft)
			{
				for each (RigidBody* r in topLeftLeft)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}

			for each(RigidBody* l in topRight)
			{
				for each (RigidBody* r in topRight)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}
			for each(RigidBody* l in topRightRight)
			{
				for each (RigidBody* r in topRightRight)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}

			for each(RigidBody* l in bottomLeft)
			{
				for each (RigidBody* r in bottomLeft)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}
			for each(RigidBody* l in bottomLeftLeft)
			{
				for each (RigidBody* r in bottomLeftLeft)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}

			for each(RigidBody* l in bottomRight)
			{
				for each (RigidBody* r in bottomRight)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}
			for each(RigidBody* l in bottomRightRight)
			{
				for each (RigidBody* r in bottomRightRight)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(r->getVel() - l->getVel());
					glm::vec3 n = glm::normalize(r->getPos() - l->getPos());
					float n2 = glm::dot(r->getVel(), n);
					float n1 = glm::dot(l->getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1 - n2) / l->getMass() + r->getMass());
					//float Jr = topPart / bottomPart;
					if (glm::fastDistance(l->getPos(), r->getPos()) < 2.0f && l != r)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						l->setVel(l->getVel() - (bottomPart / l->getMass()) * n);
						r->setVel(r->getVel() + (bottomPart / r->getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}

			topLeft.clear();
			topLeftLeft.clear();
			topRight.clear();
			topRightRight.clear();
			bottomLeft.clear();
			bottomLeftLeft.clear();
			bottomRight.clear();
			bottomRightRight.clear();
			accumulator -= dt;
			t += dt;
		}

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		//app.draw(leftWall);
		//app.draw(rightWall);
		//app.draw(topWall);
		//app.draw(bottomWall);

		//draw rb
		for each (RigidBody sphere in sphere)
		{
			app.draw(sphere.getMesh());
		}
		
		app.display();
	}
	
	app.terminate();

	return EXIT_SUCCESS;
}