#include "GLCommon.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>		// c libs
#include <stdio.h>		// c libs

#include <iostream>
#include <string>

#include "Mathf.h"
#include "PhysicsEngine.h"
#include "Scene.h"
#include "cLightManager.h"
#include "cDebugRenderer.h"
#include "cGameObject.h"
#include "cVAOManager.h"

#include "iInputHandler.h"
#include "cPhysicsInputHandler.h"
#include "cLightController.h"
#include "cLayoutController.h"
#include "cModelLoader.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

unsigned int input_id = 0;

Scene* scene;
GLFWwindow* window;
iInputHandler* pInputHandler;

void DrawObject(cGameObject* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		delete pInputHandler;
		input_id++;
		if (input_id % 3 == 1)
		{
			pInputHandler = new cLightController(*scene);
		}
		else if (input_id % 3 == 2)
		{
			pInputHandler = new cLayoutController(*scene);
		}
		else
		{
			pInputHandler = new cPhysicsInputHandler(*scene);
		}
	}

	if (pInputHandler) pInputHandler->key_callback(window, key, scancode, action, mods);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void HandleInput(GLFWwindow* window)
{
	glm::vec3 forwardVector = Mathf::get_direction_vector(scene->cameraEye, scene->cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(scene->cameraEye, scene->cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);


	// Move the camera (A & D for left and right, along the x axis)
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		Mathf::rotate_vector(-5.f, scene->cameraTarget, scene->cameraEye);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		Mathf::rotate_vector(5.f, scene->cameraTarget, scene->cameraEye);
	}

	// Move the camera (Q & E for up and down, along the y axis)
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		scene->cameraEye.y -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		scene->cameraEye.y += 0.1f;
	}

	// Move the camera (W & S for towards and away, along the z axis)
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		scene->cameraEye += forwardVector * 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		scene->cameraEye += backwardsVector * 0.1f;
	}

	if (pInputHandler) pInputHandler->HandleInput(window);
	
}

int main(void)
{

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLEngine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	// Load scene from file
	scene = Scene::LoadFromXML("physics.scene.xml");

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	PhysicsEngine phys;
	phys.GenerateAABB(scene);

	pInputHandler = new cLayoutController(*scene);

	/*cGameObject* boundary = new cGameObject;
	boundary->meshName = "cube";
	boundary->scale = glm::distance(scene->pModelLoader->min, scene->pModelLoader->max) * 2.f;
	boundary->pos = (scene->pModelLoader->min + scene->pModelLoader->max) / 2.f;
	boundary->isWireframe = true;
	boundary->uniformColour = true;
	boundary->colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	boundary->inverseMass = 0.f;
	scene->vecGameObjects.push_back(boundary);*/


#if _DEBUG
	cDebugRenderer* renderer = cDebugRenderer::Instance();
	renderer->initialize();
	phys.renderer = renderer;
#endif

	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float delta_time = 0.f;

	std::vector<float> time_buffer;

	while (!glfwWindowShouldClose(window))
	{
		previous_time = current_time;
		current_time = (float)glfwGetTime();
		delta_time = current_time - previous_time;

		// Average out the delta time to avoid randoms
		//-------------------------------------------------
		time_buffer.push_back(delta_time);
		if (time_buffer.size() > 10)
			time_buffer.erase(time_buffer.begin());

		float average = 0.f;
		unsigned int k = 0;
		for (; k < time_buffer.size(); ++k)
		{
			average += time_buffer[k];
		}
		average /= (float)k;

		/*if (average != 0.f && delta_time > 2.f * average)
		{
			average *= (float)k;
			average -= delta_time;
			average /= (float)k;
			time_buffer.erase(time_buffer.end() - 1);
		}*/
		delta_time = average;
		//------------------------------------------------

		// Handle key inputs
		HandleInput(window);

		float ratio;
		int width, height;
		//glm::mat4 m, p, v, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;


		glViewport(0, 0, width, height);

		// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Update the objects' physics
		phys.CheckCollisions(scene);
		phys.IntegrationStep(scene, /*delta_time*/delta_time);


		// **************************************************
		// **************************************************
		glm::mat4 p, v;

		// Projection matrix
		p = glm::perspective(0.6f,		// FOV
							 ratio,			// Aspect ratio
							 0.1f,			// Near clipping plane
							 1000.0f);		// Far clipping plane

		// View matrix
		v = glm::mat4(1.0f);

		v = glm::lookAt(scene->cameraEye,
						scene->cameraTarget,
						scene->upVector);
		
		
		// Loop to draw everything in the scene
		for (int index = 0; index != scene->vecGameObjects.size(); index++)
		{
			cGameObject* objPtr = scene->vecGameObjects[index];

			DrawObject(objPtr, ratio, v, p);

		}//for (int index...
		// **************************************************
		// **************************************************

#if _DEBUG

		renderer->RenderDebugObjects(v, p, 0.01f);
#endif


		 // **************************************************
		// **************************************************




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();


	// Delete everything
	delete scene;
	//delete lightManager;

	exit(EXIT_SUCCESS);
}




void DrawObject(cGameObject* objPtr, float ratio, glm::mat4 const& v, glm::mat4 const& p)
{
	GLint shaderProgID = scene->shaderProgID;
	glUseProgram(shaderProgID);

	glm::mat4 m, mvp;

	m = glm::mat4(1.0f);



	// ******* TRANSLATION TRANSFORM *********
	glm::mat4 matTrans
		= glm::translate(glm::mat4(1.0f),
						 glm::vec3(objPtr->pos.x,
								   objPtr->pos.y,
								   objPtr->pos.z));
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********



	// ******* ROTATION TRANSFORM *********
	//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
									objPtr->rotation.z,					// Angle
									glm::vec3(0.0f, 0.0f, 1.0f));
	m = m * rotateZ;

	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
									objPtr->rotation.y,	//(float)glfwGetTime(),					// Angle
									glm::vec3(0.0f, 1.0f, 0.0f));
	m = m * rotateY;

	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
									objPtr->rotation.x,	// (float)glfwGetTime(),					// Angle
									glm::vec3(1.0f, 0.0f, 0.0f));
	m = m * rotateX;
	// ******* ROTATION TRANSFORM *********



	// ******* SCALE TRANSFORM *********
	glm::mat4 scale = glm::scale(glm::mat4(1.0f),
								 glm::vec3(objPtr->scale,
										   objPtr->scale,
										   objPtr->scale));
	m = m * scale;
	// ******* SCALE TRANSFORM *********



	//mat4x4_mul(mvp, p, m);
	mvp = p * v * m;

	// Choose which shader to use
	//glUseProgram(program);
	

	GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");
	GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
	GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(p));

	GLint matModelIT_UL = glGetUniformLocation(shaderProgID, "matModelInverTrans");
	glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));



	GLint eye_loc = glGetUniformLocation(shaderProgID, "eyeLocation");
	glUniform4f(eye_loc,
				scene->cameraEye.x,
				scene->cameraEye.y,
				scene->cameraEye.z,
				1.0f
	);

	GLint diff_loc = glGetUniformLocation(shaderProgID, "diffuseColour");
	glUniform4f(diff_loc,
				objPtr->colour.x,
				objPtr->colour.y,
				objPtr->colour.z,
				objPtr->colour.w
	);

	GLint spec_loc = glGetUniformLocation(shaderProgID, "specularColour");
	glUniform4f(spec_loc,
				objPtr->specColour.x,
				objPtr->specColour.y,
				objPtr->specColour.z,
				objPtr->specIntensity
	);

	GLint isUniform_location = glGetUniformLocation(shaderProgID, "isUniform");
	glUniform1i(isUniform_location, objPtr->uniformColour);


	scene->pLightManager->Set_Light_Data(shaderProgID);


	


	// This will change the fill mode to something 
	//  GL_FILL is solid 
	//  GL_LINE is "wireframe"
	//glPointSize(15.0f);

	if (objPtr->isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	sModelDrawInfo drawInfo;
	if (scene->pVAOManager->FindDrawInfoByModelName(objPtr->meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES,
					   drawInfo.numberOfIndices,
					   GL_UNSIGNED_INT,
					   0);
		glBindVertexArray(0);
	}
}