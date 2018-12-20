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
bool impulseApplied = false;

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

	const int NUM_SPHERES = 1000;

	//RigidBody rb = RigidBody();
	
	Mesh m = Mesh::Mesh("resources/models/sphere.obj");

	RigidBody sphere[NUM_SPHERES];
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_real_distribution<float> dist(-200.0f, 200.0f);
	std::uniform_real_distribution<float> moveDir(-20.0f, 20.0f);
	for (int i = 0; i < NUM_SPHERES; i++)
	{
		sphere[i].setMesh(m);
		//rb.scale(glm::vec3(1.0f, 1.0f, 1.0f));
		Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
		sphere[i].getMesh().setShader(rbShader);
		//sphere1.setRadius(1.0f);
		sphere[i].scale(glm::vec3(1.0f, 1.0f, 1.0f));


		sphere[i].translate(glm::vec3(dist(e2), 1.0f, dist(e2)));
		sphere[i].setVel(glm::vec3(moveDir(e2), 0.0f, moveDir(e2)));
		sphere[i].setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
		sphere[i].setMass(2.0f);
	}

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

				for each(RigidBody &sSphere in sphere)
				{
					//glm::vec3 r = sphere1.getPos();
					glm::vec3 Vr = glm::normalize(sSphere.getVel() - sphere1.getVel());
					glm::vec3 n = glm::normalize(sSphere.getPos() - sphere1.getPos());
					float n2 = glm::dot(sSphere.getVel(), n);
					float n1 = glm::dot(sphere1.getVel(), n);
					//float topPart = -(1 + e) * glm::dot(Vr, n);
					float bottomPart = (2.0 * (n1-n2) / sphere1.getMass() + sSphere.getMass());
					//float Jr = topPart / bottomPart;
					if (glm::distance(sphere1.getPos(), sSphere.getPos()) < 2.0f && &sphere1 != &sSphere)
					{
						//glm::vec3 overlap = 2.0f - (sSphere.getPos() - sphere1.getPos());
						//sphere1.translate((sphere1.getVel() * -1.0f) * overlap / 2);
						//sSphere.translate((sSphere.getVel() * -1.0f) * overlap / 2);
						sphere1.setVel(sphere1.getVel() - (bottomPart / sphere1.getMass()) * n);
						sSphere.setVel(sSphere.getVel() + (bottomPart / sSphere.getMass()) * n);
						//std::cout << "Collision";
					}
				}
			}
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