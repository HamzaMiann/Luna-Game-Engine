
#include "cPhysicsInputHandler.h"
#include "Scene.h"
#include "Mathf.h"
#include "cGameObject.h"
#include "cDebugRenderer.h"
#include <glm/gtc/matrix_transform.hpp>


glm::vec3 originalBallPosition = glm::vec3(0.0f, 6.0f, 0.0f);


void cPhysicsInputHandler::HandleInput(GLFWwindow* window)
{
	cGameObject* player = _scene.vecGameObjects[1];

	

	glm::vec3 forwardVector = Mathf::get_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);

	

	//_scene.upVector = normalize(glm::cross(forwardVector, rightVector));

	//_scene.cameraEye.y += _scene.upVector.y * 3.f;

	glm::vec3 forward = (player->getQOrientation() * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	glm::vec3 right = glm::normalize((player->getQOrientation() * glm::quat(glm::vec3(0.f, glm::radians(90.f), 0.f))) * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f;
	glm::vec3 up = glm::normalize(glm::cross(forward, right));

	/*cDebugRenderer::Instance()->addLine(player->pos,
										player->pos + forward,
										glm::vec3(0.f, 1.f, 0.f), 0.1);

	cDebugRenderer::Instance()->addLine(player->pos,
										player->pos + glm::normalize(glm::cross(forward, right)),
										glm::vec3(0.f, 1.f, 0.f), 0.1);*/


	double x, y;
	glfwGetCursorPos(window, &x, &y);

	if (previousX == 0 && previousY == 0)
	{
		previousX = x;
		previousY = y;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}

	printf("%f, %f\n", x, y);

	_scene.cameraTarget = player->pos + forward;
	_scene.cameraEye = player->pos - forward * 1.5f;
	_scene.upVector = up;

	_scene.cameraEye += up * 2.f;

	float deltaX = previousX - x;
	float deltaY = previousY - y;

	//if (glfwGetKey(window, GLFW_KEY_UP))
	//{
	//	/*_scene.vecGameObjects[1]->velocity += Mathf::get_direction_vector(_scene.vecGameObjects[1]->pos,
	//																	  _scene.vecGameObjects[1]->pos + forwardVector) * 0.01f;*/
	//	//player->velocity += forward * 0.5f;
	//	player->updateOrientation(glm::vec3(3.f, 0.f, 0.f));
	//}
	//if (glfwGetKey(window, GLFW_KEY_DOWN))
	//{
	//	/*_scene.vecGameObjects[1]->velocity -= Mathf::get_direction_vector(_scene.vecGameObjects[1]->pos,
	//																	  _scene.vecGameObjects[1]->pos + forwardVector) * 0.01f;*/
	//	//player->velocity += forward * -0.5f;
	//	player->updateOrientation(glm::vec3(-3.f, 0.f, 0.f));
	//}


	player->updateOrientation(glm::vec3(deltaY * -0.1f, 0.f, 0.f));
	player->updateOrientation(glm::vec3(0.f, deltaX * 0.1f, 0.f));

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		player->velocity += forward * 0.3f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		player->velocity += forward * -0.3f;
	}
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		player->velocity += right * 0.3f;
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		player->velocity += right * -0.3f;
	}
	//if (glfwGetKey(window, GLFW_KEY_A))
	//{
	//	//_scene.vecGameObjects[1]->velocity += rightVector * 0.01f;
	//	player->updateOrientation(glm::vec3(0.f, 3.f, 0.f));
	//}
	//if (glfwGetKey(window, GLFW_KEY_D))
	//{
	//	player->updateOrientation(glm::vec3(0.f, -3.f, 0.f));
	//	//_scene.vecGameObjects[1]->velocity += leftVector * 0.01f;
	//}

	previousX = x;
	previousY = y;

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