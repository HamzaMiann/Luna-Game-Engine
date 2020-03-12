
#include "cLightController.h"
#include <Lighting/cLightManager.h>
#include <EntityManager/cEntityManager.h>
#include <cGameObject.h>
#include <iostream>
#include <sstream>
#include <Components/cRigidBody.h>

cGameObject* dbg_object;

cLightController::cLightController(Scene& scene) : _scene(scene)
{
	if (!dbg_object)
	{
		dbg_object = new cGameObject();
		dbg_object->isWireframe = true;
		dbg_object->meshName = "sphere";
		dbg_object->transform.scale = glm::vec3(0.2f);
		cRigidBody* body = dbg_object->AddComponent<cRigidBody>();
		body->inverseMass = 0.f;
		dbg_object->uniformColour = true;
		cEntityManager::Instance().AddEntity(dbg_object);
	}
	property = &cLightManager::Instance()->Lights[index]->diffuse;
}

cLightController::~cLightController()
{
	if (dbg_object)
	{
		cEntityManager::Instance().RemoveEntity(dbg_object);
		/*_scene.vecGameObjects.erase(
			std::find(_scene.vecGameObjects.begin(),
					  _scene.vecGameObjects.end(),
					  dbg_object
			)
		);*/
		//delete dbg_object;
		//dbg_object = nullptr;
	}
}

void cLightController::HandleInput(GLFWwindow * window)
{
	if (cLightManager::Instance()->Lights.size() == 0) return;

	if (glfwGetKey(window, GLFW_KEY_UP))
	{
		cLightManager::Instance()->Lights[index]->position.z += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN))
	{
		cLightManager::Instance()->Lights[index]->position.z -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT))
	{
		cLightManager::Instance()->Lights[index]->position.x += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		cLightManager::Instance()->Lights[index]->position.x -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_U))
	{
		cLightManager::Instance()->Lights[index]->position.y += 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_O))
	{
		cLightManager::Instance()->Lights[index]->position.y -= 0.1f;
	}

	if (glfwGetKey(window, GLFW_KEY_1))
	{
		property->x *= 1.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_2))
	{
		property->x *= 0.9f;
	}
	if (glfwGetKey(window, GLFW_KEY_3))
	{
		property->y *= 1.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_4))
	{
		property->y *= 0.9f;
	}
	if (glfwGetKey(window, GLFW_KEY_5))
	{
		property->z *= 1.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_6))
	{
		property->z *= 0.9f;
	}
	if (glfwGetKey(window, GLFW_KEY_7))
	{
		property->z *= 1.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_8))
	{
		property->z *= 0.9f;
	}

	std::ostringstream oss;
	switch (property_index)
	{
	case 0: oss << "diffuse(";		break;
	case 1: oss << "specular(";		break;
	case 2: oss << "atten(";		break;
	case 3: oss << "direction(";	break;
	case 4: oss << "param1(";		break;
	}

	oss << property->x << ",";
	oss << property->y << ",";
	oss << property->z << ",";
	oss << property->w << ")";

	glfwSetWindowTitle(window, oss.str().c_str());

	if (dbg_object)
		dbg_object->transform.pos = cLightManager::Instance()->Lights[index]->position;
}

void cLightController::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (cLightManager::Instance()->Lights.size() == 0) return;

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		index++;
		if (index >= cLightManager::Instance()->Lights.size())
		{
			index = 0;
		}
		switch (property_index)
		{
		case 0: property = &cLightManager::Instance()->Lights[index]->diffuse;  break;
		case 1: property = &cLightManager::Instance()->Lights[index]->specular;  break;
		case 2: property = &cLightManager::Instance()->Lights[index]->atten;  break;
		case 3: property = &cLightManager::Instance()->Lights[index]->direction;  break;
		case 4: property = &cLightManager::Instance()->Lights[index]->param1;  break;
		}
	}

	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		property_index++;
		property_index %= 5;
		switch (property_index)
		{
		case 0: property = &cLightManager::Instance()->Lights[index]->diffuse;  break;
		case 1: property = &cLightManager::Instance()->Lights[index]->specular;  break;
		case 2: property = &cLightManager::Instance()->Lights[index]->atten;  break;
		case 3: property = &cLightManager::Instance()->Lights[index]->direction;  break;
		case 4: property = &cLightManager::Instance()->Lights[index]->param1;  break;
		}
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		if (cLightManager::Instance()->Lights[index]->param2.x == 1.f)
		{
			cLightManager::Instance()->Lights[index]->param2.x = 0.f;
		}
		else
		{
			cLightManager::Instance()->Lights[index]->param2.x = 1.f;
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
