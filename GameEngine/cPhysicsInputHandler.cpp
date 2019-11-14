
#include "cPhysicsInputHandler.h"
#include "Scene.h"
#include "Mathf.h"
#include "cGameObject.h"
#include "cDebugRenderer.h"

glm::vec3 originalBallPosition = glm::vec3(0.0f, 6.0f, 0.0f);

void cPhysicsInputHandler::HandleInput(GLFWwindow* window)
{
	cGameObject* player = _scene.vecGameObjects[1];

	//_scene.cameraTarget = glm::vec3(glm::normalize(player->getQOrientation() * glm::vec4(0.f, 0.f, 1.f, 1.f)) * 5.f) + player->pos;
	//_scene.cameraEye = player->pos - (_scene.cameraTarget - player->pos) * 3.f;

	glm::vec3 forwardVector = Mathf::get_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(_scene.cameraEye, _scene.cameraTarget);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);

	

	//_scene.upVector = normalize(glm::cross(forwardVector, rightVector));

	//_scene.cameraEye.y += _scene.upVector.y * 3.f;

	glm::vec3 forward = player->getQOrientation() * glm::vec4(0.f, 0.f, 1.f, 1.f);


	cDebugRenderer::Instance()->addLine(player->pos,
										normalize(glm::cross(forward, Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forward))),
										glm::vec3(0.f, 1.f, 0.f), 0.1);

	cDebugRenderer::Instance()->addLine(player->pos,
										glm::vec4(player->pos, 1.f) + player->getQOrientation() * glm::vec4(0.f, 0.f, 10.f, 1.f),
										glm::vec3(0.f, 1.f, 0.f), 0.1);


	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		/*_scene.vecGameObjects[1]->velocity += Mathf::get_direction_vector(_scene.vecGameObjects[1]->pos,
																		  _scene.vecGameObjects[1]->pos + forwardVector) * 0.01f;*/
		//player->velocity += forward * 0.5f;
		player->updateOrientation(glm::vec3(3.f, 0.f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		/*_scene.vecGameObjects[1]->velocity -= Mathf::get_direction_vector(_scene.vecGameObjects[1]->pos,
																		  _scene.vecGameObjects[1]->pos + forwardVector) * 0.01f;*/
		//player->velocity += forward * -0.5f;
		player->updateOrientation(glm::vec3(-3.f, 0.f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		//_scene.vecGameObjects[1]->velocity += rightVector * 0.01f;
		player->updateOrientation(glm::vec3(0.f, 3.f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		player->updateOrientation(glm::vec3(0.f, -3.f, 0.f));
		//_scene.vecGameObjects[1]->velocity += leftVector * 0.01f;
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