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
	rb.setVel(glm::vec3(2.0f, 0.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
	rb.setMass(2.0f);

	//rb.addForce(&g);

	//std::cout << glm::to_string(rb.getInvInertia());
	
	
	
	
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

			rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
			glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
			glm::mat3 R = glm::mat3(rb.getRotate());

			R += dt * angVelSkew * R;
			R = glm::orthonormalize(R);
			rb.setRotate(glm::mat4(R));

			rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel(), t, dt));
			rb.setVel(rb.getVel() + (dt * rb.getAcc()));

			rb.translate(rb.getVel() * dt);
			
			// IMUPLSES HERE
			if (t > 2.0f && !impulseApplied)
			{
				glm::vec3 impulse = glm::vec3(-4.0f, 0.0f, 0.0f);
				glm::vec3 CoM = rb.getPos();
				glm::vec3 impulsePoint = CoM + glm::vec3(0.0f, -1.0f, 0.0f);
				rb.setVel(rb.getVel() + (impulse / rb.getMass()));
				
				rb.setAngVel(rb.getAngVel() + rb.getInvInertia() * (glm::cross((impulsePoint - rb.getPos()), impulse)));
				impulseApplied = true;
			}
			
			//COLLISION IMPULSE HERE
			//glm::vec4 rbPos;
			//for (Vertex v : rb.getMesh().getVertices())
			//{
			//	rbPos = rb.getMesh().getModel() * glm::vec4(v.getCoord(), 1.0f);

			//	if (rbPos.y < plane.getPos().y)
			//	{
			//		vt.push_back(rbPos);
			//		std::cout << std::endl << "Amount: " << vt.size() << std::endl;
			//		std::cout << glm::to_string(rbPos);
			//	}

			//}

			//if (vt.size() > 0)
			//{
			//	
			//	for (int i = 0; i < vt.size(); i++)
			//	{
			//		vAvg += vt.at(i);
			//	}
			//	vAvg /= vt.size();
			//	
			//	std::cout << std::endl << "AVERAGE: " << glm::to_string(vAvg) << std::endl;
			//	
			//	rb.translate(glm::vec3(0.0f, 0.04f, 0.0f));
			//	float e = 0.7f;
			//	glm::vec3 n = glm::vec3(0.0f, 1.0f, 0.0f);	
			//	glm::vec3 r = vAvg - rb.getPos();
			//	glm::vec3 Vr = rb.getVel() + glm::cross(rb.getAngVel(), r);

			//	//glm::mat3 i = rb.getRotate() * rb.getInvInertia() * glm::transpose(rb.getInvInertia());

			//	float topPart = -(1 + e) * glm::dot(Vr, n);

			//	float bottomPart = (1 / rb.getMass()) + glm::dot(n, glm::cross(rb.getInvInertia() * glm::cross(r, n), r));

			//	float Jr = topPart / bottomPart;

			//	rb.setVel(rb.getVel() + (Jr / rb.getMass()) * n);
			//	rb.setAngVel(rb.getAngVel() + Jr * rb.getInvInertia() * (glm::cross(r, n)));

			//	vt.clear();
			//	vAvg = glm::vec3(0.0f, 0.0f, 0.0f);
			//	//rb.translate(glm::vec3(0.0f, 1.0f, 0.0f));
			//	//rb.setAcc(glm::vec3(0.0f, 0.0f, 0.0f));
			//	//rb.setVel(glm::vec3(0.0f, 0.0f, 0.0f));
			//	//rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));
			//}
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