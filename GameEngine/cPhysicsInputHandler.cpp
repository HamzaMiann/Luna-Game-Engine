
#include "cPhysicsInputHandler.h"
#include "Scene.h"
#include "Mathf.h"
#include "cGameObject.h"

glm::vec3 originalBallPosition = glm::vec3(0.0f, 6.0f, 0.0f);

void cPhysicsInputHandler::HandleInput(GLFWwindow* window)
{

	glm::vec3 forwardVector = Mathf::get_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		_scene.vecGameObjects[1]->velocity += Mathf::get_direction_vector(_scene.vecGameObjects[1]->pos,
																		  _scene.vecGameObjects[1]->pos + forwardVector) * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		_scene.vecGameObjects[1]->velocity -= Mathf::get_direction_vector(_scene.vecGameObjects[1]->pos,
																		  _scene.vecGameObjects[1]->pos + forwardVector) * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		_scene.vecGameObjects[1]->velocity += rightVector * 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		_scene.vecGameObjects[1]->velocity += leftVector * 0.01f;
	}

}


void cPhysicsInputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		_scene.vecGameObjects[1]->velocity += _scene.upVector * 2.f;
		//_scene.pAudioEngine->PlaySound("bounce");
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		_scene.vecGameObjects[1]->pos = originalBallPosition;
		_scene.vecGameObjects[1]->velocity = glm::vec3(0.f);
		//_scene.pAudioEngine->PlaySound("respawn");
	}
}