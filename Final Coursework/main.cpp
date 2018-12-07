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

	const int NUM_SPHERES = 30;

	//RigidBody rb = RigidBody();
	
	Mesh m = Mesh::Mesh("resources/models/sphere.obj");

	RigidBody sphere[NUM_SPHERES];

	for (int i = 0; i < NUM_SPHERES; i++)
	{
		sphere[i].setMesh(m);
		//rb.scale(glm::vec3(1.0f, 1.0f, 1.0f));
		Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
		sphere[i].getMesh().setShader(rbShader);
		//sphere[i].setRadius(1.0f);
		sphere[i].scale(glm::vec3(1.0f, 1.0f, 1.0f));

		std::random_device rd;
		std::mt19937 e2(rd());
		std::uniform_real_distribution<float> dist(-20.0f, 20.0f);
		std::uniform_real_distribution<float> moveDir(-10.0f, 10.0f);
		sphere[i].translate(glm::vec3(dist(e2), 1.0f, dist(e2)));
		sphere[i].setVel(glm::vec3(moveDir(e2), 0.0f, moveDir(e2)));
		sphere[i].setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
		sphere[i].setMass(2.0f);
	}


	std::vector<glm::vec3> vt;
	glm::vec3 vAvg = glm::vec3(0.0f, 0.0f, 0.0f);

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
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
			for (int i = 0; i < NUM_SPHERES; i++)
			{
				sphere[i].setAngVel(sphere[i].getAngVel() + dt * sphere[i].getAngAcc());
				glm::mat3 angVelSkew = glm::matrixCross3(sphere[i].getAngVel());
				glm::mat3 R = glm::mat3(sphere[i].getRotate());

				R += dt * angVelSkew * R;
				R = glm::orthonormalize(R);
				sphere[i].setRotate(glm::mat4(R));

				sphere[i].setAcc(sphere[i].applyForces(sphere[i].getPos(), sphere[i].getVel(), t, dt));
				sphere[i].setVel(sphere[i].getVel() + (dt * sphere[i].getAcc()));

				sphere[i].translate(sphere[i].getVel() * dt);
			
				glm::vec3 n;
			//std::cout << "Reaches1";
			//COLLISION IMPULSE HERE

				float e = 1.0f;
				glm::vec3 r = sphere[i].getPos() + 1.0f;
				glm::vec3 Vr = sphere[i].getVel() + glm::cross(sphere[i].getAngVel(), r);

				//glm::mat3 i = sphere[i].getRotate() * sphere[i].getInvInertia() * glm::transpose(sphere[i].getInvInertia());

				float topPart = -(1 + e) * glm::dot(Vr, n);

				float bottomPart = (1 / sphere[i].getMass()) + glm::dot(n, glm::cross(sphere[i].getInvInertia() * glm::cross(r, n), r));

				float Jr = topPart / bottomPart;
				if (sphere[i].getPos().x - 1.0f <= -30.0f)
				{
					n = glm::vec3(1.0f, 0.0f, 0.0f);
					sphere[i].translate(glm::vec3(0.2f, 0.0f, 0.0f));
					sphere[i].setVel(sphere[i].getVel() + (Jr / sphere[i].getMass()) * n);
				}
				else if (sphere[i].getPos().x + 1.0f >= 30.0f)
				{
					n = glm::vec3(-1.0f, 0.0f, 0.0f);
					sphere[i].translate(glm::vec3(-0.2f, 0.0f, 0.0f));
					sphere[i].setVel(sphere[i].getVel() + (Jr / sphere[i].getMass()) * n);
				}
				else if (sphere[i].getPos().z + 1.0f >= 30.0f)
				{
					n = glm::vec3(0.0f, 0.0f, -1.0f);
					sphere[i].translate(glm::vec3(0.0f, 0.0f, -0.2f));
					sphere[i].setVel(sphere[i].getVel() + (Jr / sphere[i].getMass()) * n);
				}
				else if(sphere[i].getPos().z - 1.0f <= -30.0f)
				{
					n = glm::vec3(0.0f, 0.0f, 1.0f);
					sphere[i].translate(glm::vec3(0.0f, 0.0f, 0.2f));
					sphere[i].setVel(sphere[i].getVel() + (Jr / sphere[i].getMass()) * n);
				}

				//std::cout << "REACHED 2";
				
				//sphere[i].setVel(sphere[i].getVel() + (Jr / sphere[i].getMass()) * n);
				//sphere[i].setAngVel(sphere[i].getAngVel() + Jr * sphere[i].getInvInertia() * (glm::cross(r, n)));

				vt.clear();
				vAvg = glm::vec3(0.0f, 0.0f, 0.0f);
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
		app.draw(leftWall);
		app.draw(rightWall);
		app.draw(topWall);
		app.draw(bottomWall);

		//draw rb
		for (int i = 0; i < NUM_SPHERES; i++)
		{
			app.draw(sphere[i].getMesh());
		}
		
		app.display();
	}
	
	app.terminate();

	return EXIT_SUCCESS;
}