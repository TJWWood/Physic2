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
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);

	// time
	GLfloat firstFrame = (GLfloat)glfwGetTime();

	double t = 0.0;
	const double dt = 0.01;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;

	Gravity g = Gravity(glm::vec3(0.0f, -9.8f, 0.0f));


	RigidBody rb = RigidBody();
	Mesh m = Mesh::Mesh(Mesh::CUBE);
	rb.setMesh(m);
	rb.scale(glm::vec3(1.0f, 3.0f, 1.0f));
	Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	rb.getMesh().setShader(rbShader);
	rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	rb.setVel(glm::vec3(0.0f, -3.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 3.0f));
	rb.setMass(3.0f);

	//rb.addForce(&g);


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

			rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
			glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
			glm::mat3 R = glm::mat3(rb.getRotate());

			R += dt * angVelSkew * R;
			R = glm::orthonormalize(R);
			rb.setRotate(glm::mat4(R));

			rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel(), t, dt));
			//rb.setVel(rb.getVel() + (dt * rb.getAcc()));

			rb.translate(rb.getVel() * dt);

			for each (Vertex v in rb.getMesh().getVertices())
			{
				glm::vec4 rbPos = rb.getMesh().getModel() * glm::vec4(v.getCoord(), 1.0f);
				if (rbPos.y <= plane.getPos().y)
				{
					rb.translate(glm::vec3(0.0f,1.0f,0.0f));
					float e = 1.0f;
					glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);
					glm::vec3 Vr = rb.getVel() + rb.getAngVel();
					glm::vec3 r = v.getCoord() - rb.getPos();
					Vr = glm::cross(Vr, r);

					glm::mat3 i = rb.getRotate() * rb.getInvInertia() * glm::transpose(rb.getInvInertia());

					float topPart = -(1 + e) * glm::dot(Vr, n);
					glm::vec3 cross1 = glm::cross(r, n);
					cross1 = i * cross1;
					glm::vec3 cross2 = glm::cross(cross1, r);

					float bottomPart = -rb.getMass() + glm::dot(n, (cross2));

					float Jr = topPart / bottomPart;

					rb.setVel(rb.getVel() + (Jr / rb.getMass() * n));
					rb.setAngVel(rb.getAngVel() + (Jr * rb.getInvInertia() * (glm::cross(r, n))));
					

					//rb.setAcc(glm::vec3(0.0f, 0.0f, 0.0f));
					//rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
					//rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
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

		//draw rb
		app.draw(rb.getMesh());


		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}