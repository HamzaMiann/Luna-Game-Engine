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
#include <sstream>
#include <string>

#include "Mathf.h"
#include "PhysicsEngine.h"
#include "Scene.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

Scene* scene;
GLFWwindow* window;

AUDIO_ID current_sound_id = 0;

glm::vec3 originalBallPosition = glm::vec3(0.0f, 6.0f, 0.0f);

void DrawObject(cGameObject* objPtr, float ratio);


static std::string get_sound_details(AudioEngine::Sound* sound, std::string friendlyName)
{
	std::ostringstream ss;

	ss << '[' << sound->get_name() << "] ";
	unsigned int seconds = sound->get_position() / 1000;
	unsigned int minutes = seconds / 60;
	unsigned int sec = seconds - (minutes * 60);
	unsigned int size = sound->get_size() / 1000;
	unsigned int size_minutes = size / 60;
	unsigned int size_sec = size - (size_minutes * 60);
	ss << "format(" << sound->get_format() << ") ";
	ss << "type(" << sound->get_type() << ") ";
	ss << "[" << minutes << ":" << sec << " / " << size_minutes << ":" << size_sec << "] ";
	ss << "Vol(" << sound->get_volume() << ") ";
	ss << "Fq(" << sound->get_frequency() << ") ";
	ss << "Pan(" << sound->get_pan() << ") ";

	return ss.str();
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		scene->vecGameObjects[1]->velocity += scene->upVector * 2.f;
		scene->pAudioEngine->PlaySound("bounce");
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		scene->vecGameObjects[1]->positionXYZ = originalBallPosition;
		scene->vecGameObjects[1]->velocity = glm::vec3(0.f);
		scene->pAudioEngine->PlaySound("respawn");
	}

	if (scene->pAudioEngine && scene->pAudioEngine->Num_Sounds() > 0)
	{
		int max = scene->pAudioEngine->Num_Sounds();
		
		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		{
			current_sound_id++;
			if (current_sound_id >= max) current_sound_id = 0;
		}
		
	}

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

	if (scene->pAudioEngine && scene->pAudioEngine->Num_Sounds() > 0)
	{
		AudioEngine::Sound* current_sound = scene->pAudioEngine->GetSound(current_sound_id);
		if (glfwGetKey(window, GLFW_KEY_1))
		{
			current_sound->set_volume(current_sound->get_volume() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_2))
		{
			current_sound->set_volume(current_sound->get_volume() + 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_3))
		{
			current_sound->set_pitch(current_sound->get_pitch() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_4))
		{
			current_sound->set_pitch(current_sound->get_pitch() + 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_5))
		{
			current_sound->set_pan(current_sound->get_pan() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_6))
		{
			current_sound->set_pan(current_sound->get_pan() + 0.01f);
		}

	}

	// Move the camera (A & D for left and right, along the x axis)
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		Mathf::rotate_vector(-5.f, scene->cameraTarget, scene->cameraEye);
		//cameraEye.x -= 0.1f;		// Move the camera -0.01f units
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		Mathf::rotate_vector(5.f, scene->cameraTarget, scene->cameraEye);
		//cameraEye.x += 0.1f;		// Move the camera +0.01f units
	}

	// Move the camera (Q & E for up and down, along the y axis)
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		scene->cameraEye.y -= 0.1f;		// Move the camera -0.01f units
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		scene->cameraEye.y += 0.1f;		// Move the camera +0.01f units
	}

	// Move the camera (W & S for towards and away, along the z axis)
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		scene->cameraEye += forwardVector * 0.1f;
		//cameraEye.z += 0.5f;		// Move the camera -0.01f units
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		scene->cameraEye += backwardsVector * 0.1f;
		//cameraEye.z -= 0.5f;		// Move the camera +0.01f units
	}


	if (glfwGetKey(window, GLFW_KEY_J))
	{
		scene->LightLocation.x -= 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_K))
	{
		scene->LightLocation.x += 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		scene->vecGameObjects[1]->velocity += Mathf::get_direction_vector(scene->vecGameObjects[1]->positionXYZ,
																		  scene->vecGameObjects[1]->positionXYZ + forwardVector) * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		scene->vecGameObjects[1]->velocity -= Mathf::get_direction_vector(scene->vecGameObjects[1]->positionXYZ,
																		  scene->vecGameObjects[1]->positionXYZ + forwardVector) * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		scene->vecGameObjects[1]->velocity += rightVector * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		scene->vecGameObjects[1]->velocity += leftVector * 0.01f;
	}

	
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
	scene = Scene::LoadFromXML("scene1.scene.xml");


	// At this point, the model is loaded into the GPU
	// Load up my "scene" 

	cGameObject sphere;
	sphere.meshName = "sphere";
	sphere.Collider = SPHERE;
	sphere.positionXYZ = originalBallPosition;
	sphere.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	sphere.scale = 0.5f;
	sphere.objectColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Set the sphere's initial velocity, etc.
	sphere.velocity = glm::vec3(-0.1f, 0.f, 0.f);
	sphere.acceleration = glm::vec3(0.f);
	sphere.inverseMass = 1.f;

	cGameObject debugSphere;
	debugSphere.meshName = "sphere";
	debugSphere.Collider = SPHERE;
	debugSphere.positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	debugSphere.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	debugSphere.scale = .2f;
	debugSphere.objectColourRGBA = glm::vec4(0.f, 1.f, 0.f, 1.0f);

	cGameObject cube;
	cube.meshName = "cube";
	cube.Collider = MESH;
	cube.positionXYZ = glm::vec3(0.0f, -0.5f, 0.0f);
	cube.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	cube.scale = 10.f;
	cube.objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// Set the cube's initial velocity, etc.
	cube.inverseMass = 0.f;	// does not move

	cGameObject wall;
	wall.meshName = "plane_wall";
	wall.Collider = MESH;
	wall.positionXYZ = glm::vec3(0.0f, 0.f, 2.0f);
	wall.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	wall.scale = 10.f;
	wall.objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// Set the cube's initial velocity, etc.
	wall.inverseMass = 0.f;	// does not move

	cGameObject wall2;
	wall2.meshName = "plane_wall2";
	wall2.Collider = MESH;
	wall2.positionXYZ = glm::vec3(2.0f, 0.f, 0.0f);
	wall2.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	wall2.scale = 10.f;
	wall2.objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// Set the cube's initial velocity, etc.
	wall2.inverseMass = 0.f;	// does not move


	cGameObject angle_wall;
	angle_wall.meshName = "plane_angle";
	angle_wall.Collider = MESH;
	angle_wall.positionXYZ = glm::vec3(-2.0f, 0.f, 0.0f);
	angle_wall.rotationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	angle_wall.scale = 10.f;
	angle_wall.objectColourRGBA = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	// Set the cube's initial velocity, etc.
	angle_wall.inverseMass = 0.f;	// does not move


	scene->vecGameObjects.push_back(&cube);
	scene->vecGameObjects.push_back(&sphere);
	scene->vecGameObjects.push_back(&wall);
	scene->vecGameObjects.push_back(&wall2);
	scene->vecGameObjects.push_back(&angle_wall);


	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	float shipVelocityZ = 0.1f;
	float shipAccelz = 100.f;

	// Initialize audio engine
	
	scene->pAudioEngine->PlaySound("music");
	scene->pAudioEngine->PlaySound("rain");


	PhysicsEngine phys;


	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float delta_time = 0.f;

	while (!glfwWindowShouldClose(window))
	{
		previous_time = current_time;
		current_time = (float)glfwGetTime();
		delta_time = current_time - previous_time;

		// Handle key inputs
		HandleInput(window);

		float ratio;
		int width, height;
		glm::mat4 m, p, v, mvp;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;

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


		glViewport(0, 0, width, height);

		// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Update the objects' physics
		phys.CheckCollisions(scene);
		phys.IntegrationStep(scene, /*delta_time*/delta_time);

		std::string friendlyName = scene->pAudioEngine->Get_Name(current_sound_id);
		glfwSetWindowTitle(window, get_sound_details(scene->pAudioEngine->GetSound(current_sound_id), friendlyName).c_str());

		// **************************************************
		// **************************************************
		// Loop to draw everything in the scene
		for (int index = 0; index != scene->vecGameObjects.size(); index++)
		{
			cGameObject* objPtr = scene->vecGameObjects[index];
			
			DrawObject(objPtr, ratio);

		}//for (int index...

		float closestDistanceSoFar = FLT_MAX;
		glm::vec3 closestPoint = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

		//FindClosestPointToMesh(*scene, closestDistanceSoFar, closestPoint, normal, scene->vecGameObjects[0], scene->vecGameObjects[1]);

		//debugSphere.positionXYZ = closestPoint;
		//DrawObject(&debugSphere, ratio);

		 // **************************************************
		// **************************************************




		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();


	// Delete everything
	delete scene;

	exit(EXIT_SUCCESS);
}




void DrawObject(cGameObject* objPtr, float ratio)
{
	glm::mat4 m, p, v, mvp;

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

	//cGameObject* objPtr = scene->vecGameObjects[index];
	//         mat4x4_identity(m);
	m = glm::mat4(1.0f);



	// ******* TRANSLATION TRANSFORM *********
	glm::mat4 matTrans
		= glm::translate(glm::mat4(1.0f),
						 glm::vec3(objPtr->positionXYZ.x,
								   objPtr->positionXYZ.y,
								   objPtr->positionXYZ.z));
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********



	// ******* ROTATION TRANSFORM *********
	//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
									objPtr->rotationXYZ.z,					// Angle
									glm::vec3(0.0f, 0.0f, 1.0f));
	m = m * rotateZ;

	glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
									objPtr->rotationXYZ.y,	//(float)glfwGetTime(),					// Angle
									glm::vec3(0.0f, 1.0f, 0.0f));
	m = m * rotateY;

	glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
									objPtr->rotationXYZ.x,	// (float)glfwGetTime(),					// Angle
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
	GLint shaderProgID = scene->shaderProgID;
	glUseProgram(shaderProgID);

	GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");
	GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
	GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(m));
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(p));



	// Find the location of the uniform variable newColour
	GLint newColour_location = glGetUniformLocation(shaderProgID, "newColour");

	glUniform3f(newColour_location,
				objPtr->objectColourRGBA.r,
				objPtr->objectColourRGBA.g,
				objPtr->objectColourRGBA.b);

	GLint newColourRed_UL = glGetUniformLocation(shaderProgID, "newColourRed");
	GLint newColourGreen_UL = glGetUniformLocation(shaderProgID, "newColourGreen");
	GLint newColourBlue_UL = glGetUniformLocation(shaderProgID, "newColourBlue");

	glUniform1f(newColourRed_UL, objPtr->objectColourRGBA.r);
	glUniform1f(newColourGreen_UL, objPtr->objectColourRGBA.g);
	glUniform1f(newColourBlue_UL, objPtr->objectColourRGBA.b);


	GLint lighPosition_UL = glGetUniformLocation(shaderProgID, "lightPosition");
	glUniform3f(lighPosition_UL,
				scene->LightLocation.x,
				scene->LightLocation.y,
				scene->LightLocation.z);



	// This will change the fill mode to something 
	//  GL_FILL is solid 
	//  GL_LINE is "wireframe"
	//glPointSize(15.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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