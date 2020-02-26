
#include <InputManager.h>
#include <glm/glm_common.h>
#include <Camera.h>
#include <_GL/Window.h>
#include <string>
#include <algorithm>
#include <cGameObject.h>
#include <Shader/Shader.h>
#include <EntityManager/cEntityManager.h>
#include <Mesh/cModelLoader.h>
#include <Mesh/cVAOManager.h>
//#define CAMERA_CONTROL

std::vector<int> Input::keys_released;
std::vector<int> Input::keys_pressed;
Input::Scroll Input::scroll;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (action == GLFW_PRESS)
	{
		Input::keys_pressed.push_back(key);
	}
	
	if (action == GLFW_RELEASE)
	{
		Input::keys_released.push_back(key);
	}

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input::scroll.xoffset += xoffset;
	Input::scroll.yoffset += yoffset;
}


std::string random_string(size_t length)
{
	auto randchar = []() -> char
	{
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string str(length, 0);
	std::generate_n(str.begin(), length, randchar);
	return str;
}

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	for (int i = 0; i < count; ++i)
	{
		std::string file = paths[i];
		std::string name = random_string(10);
		Shader* shader = Shader::FromName("basic");
		cMesh* mesh = new cMesh;
		cModelLoader::Instance().LoadModel(file, name, *mesh);
		sModelDrawInfo* drawInfo = new sModelDrawInfo;
		cVAOManager::Instance().LoadModelIntoVAO(name, *mesh, *drawInfo, shader->GetID());

		cGameObject* object = new cGameObject;
		object->shader = shader;
		object->meshName = name;
		cEntityManager::Instance().AddEntity(object);
	}
}

void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

void HandleInput(GLFWwindow* window)
{
#ifdef CAMERA_CONTROL
	vec3 forwardVector = Mathf::get_direction_vector(Camera::main_camera->Eye, Camera::main_camera->Target);
	vec3 backwardsVector = Mathf::get_reverse_direction_vector(Camera::main_camera->Eye, Camera::main_camera->Target);
	vec3 rightVector = Mathf::get_rotated_vector(-90.f, vec3(0.f), forwardVector);
	vec3 leftVector = Mathf::get_rotated_vector(90.f, vec3(0.f), forwardVector);


	// Move the camera (A & D for left and right, along the x axis)
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		Mathf::rotate_vector(-5.f, Camera::main_camera->Target, Camera::main_camera->Eye);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		Mathf::rotate_vector(5.f, Camera::main_camera->Target, Camera::main_camera->Eye);
	}

	// Move the camera (Q & E for up and down, along the y axis)
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		Camera::main_camera->Eye.y -= glm::length(forwardVector) * 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		Camera::main_camera->Eye.y += glm::length(forwardVector) * 0.05f;
	}

	// Move the camera (W & S for towards and away, along the z axis)
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		Camera::main_camera->Eye += forwardVector * 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		Camera::main_camera->Eye += backwardsVector * 0.1f;
	}
#endif


}

bool Input::KeyDown(int key)
{
	for (size_t i = 0; i < keys_pressed.size(); ++i)
	{
		if (keys_pressed[i] == key) return true;
	}
	return false;
}

bool Input::KeyUp(int key)
{
	for (size_t i = 0; i < keys_released.size(); ++i)
	{
		if (keys_released[i] == key) return true;
	}
	return false;
}

bool Input::GetKey(int key)
{
	return glfwGetKey(global::window, key);
}

void Input::LockCursor()
{
	glfwSetInputMode(global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::UnlockCursor()
{
	glfwSetInputMode(global::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::CursorPosition(double& x, double& y)
{
	glfwGetCursorPos(global::window, &x, &y);
}

bool Input::GetMouseButton(int button)
{
	return glfwGetMouseButton(global::window, button);
}

float Input::GetScrollX()
{
	return scroll.xoffset;
}

float Input::GetScrollY()
{
	return scroll.yoffset;
}

void Input::ClearBuffer()
{
	keys_pressed.clear();
	keys_released.clear();
	scroll.xoffset = 0.0;
	scroll.yoffset = 0.0;
}
