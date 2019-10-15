
#include "cLightController.h"
#include "cLightManager.h"
#include "cGameObject.h"
#include <iostream>

cGameObject* dbg_object;

cLightController::cLightController(Scene& scene) : _scene(scene)
{
#if _DEBUG
	dbg_object = new cGameObject();
	dbg_object->isWireframe = true;
	dbg_object->meshName = "sphere";
	dbg_object->scale = 0.2f;
	dbg_object->inverseMass = 0.f;
	dbg_object->uniformColour = true;
	_scene.vecGameObjects.push_back(dbg_object);
#endif
}

void cLightController::HandleInput(GLFWwindow * window)
{
	if (this->_scene.pLightManager->Lights.size() == 0) return;

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		this->_scene.pLightManager->Lights[index]->position.z += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		this->_scene.pLightManager->Lights[index]->position.z -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		this->_scene.pLightManager->Lights[index]->position.x += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		this->_scene.pLightManager->Lights[index]->position.x -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_PAGE_UP))
	{
		this->_scene.pLightManager->Lights[index]->position.y += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
	{
		this->_scene.pLightManager->Lights[index]->position.y -= 0.1f;
	}

	if (dbg_object)
		dbg_object->pos = this->_scene.pLightManager->Lights[index]->position;
}

void cLightController::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (this->_scene.pLightManager->Lights.size() == 0) return;

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		index++;
		if (index >= this->_scene.pLightManager->Lights.size())
		{
			index = 0;
		}
	}

	if (key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
	{
		if (_scene.SaveLights())
			std::cout << "Lights Saved!" << std::endl;
		else
			std::cout << "Unable to save lights..." << std::endl;
	}
}
