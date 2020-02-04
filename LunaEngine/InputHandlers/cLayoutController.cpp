
#include "cLayoutController.h"
#include "cGameObject.h"
#include <iostream>
#include <EntityManager/cEntityManager.h>

glm::vec3 original_target_position;

cLayoutController::cLayoutController(Scene& scene) : _scene(scene)
{
	original_target_position = _scene.camera.Target;
}

cLayoutController::~cLayoutController()
{
	_scene.camera.Target = original_target_position;
}

void cLayoutController::HandleInput(GLFWwindow * window)
{
	float speed = 10.f;

	auto& objects = cEntityManager::Instance()->GetEntities();

	if (objects.size() == 0) return;

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		objects[index]->transform.pos.z += 0.1f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		objects[index]->transform.pos.z -= 0.1f * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		objects[index]->transform.pos.x += 0.1f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		objects[index]->transform.pos.x -= 0.1f * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_T))
	{
		objects[index]->transform.pos.y += 0.1f * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_Y))
	{
		objects[index]->transform.pos.y -= 0.1f * speed;
	}

	if (glfwGetKey(window, GLFW_KEY_I))
	{
		objects[index]->transform.UpdateEulerRotation(glm::vec3(0.f, 0.f, 0.1f));
	}
	if (glfwGetKey(window, GLFW_KEY_K))
	{
		objects[index]->transform.UpdateEulerRotation(glm::vec3(0.f, 0.f, -0.1f));
	}

	if (glfwGetKey(window, GLFW_KEY_J))
	{
		objects[index]->transform.UpdateEulerRotation(glm::vec3(0.1f, 0.f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_L))
	{
		objects[index]->transform.UpdateEulerRotation(glm::vec3(-0.1f, 0.f, 0.f));
	}

	if (glfwGetKey(window, GLFW_KEY_U))
	{
		objects[index]->transform.UpdateEulerRotation(glm::vec3(0.f, 0.1f, 0.f));
	}
	if (glfwGetKey(window, GLFW_KEY_O))
	{
		objects[index]->transform.UpdateEulerRotation(glm::vec3(0.f, -0.1f, 0.f));
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL))
	{
		objects[index]->transform.scale *= 1.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_MINUS))
	{
		objects[index]->transform.scale *= 0.95f;
	}

	_scene.camera.Target = objects[index]->transform.pos;


}

void cLayoutController::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	auto& objects = cEntityManager::Instance()->GetEntities();

	if (objects.size() == 0) return;

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		index++;
		if (index >= objects.size())
		{
			index = 0;
		}
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		objects[index]->isWireframe = !objects[index]->isWireframe;
		objects[index]->uniformColour = !objects[index]->uniformColour;
	}

	/*if (key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		if (_scene.SaveLayout())
			std::cout << "Layout Saved!" << std::endl;
		else
			std::cout << "Unable to save layout..." << std::endl;
	}*/
}
