
#include <InputManager.h>
#include <glm/glm_common.h>
#include <Physics/Mathf.h>
#include <Camera.h>
#include <_GL\Window.h>
#define CAMERA_CONTROL

std::vector<int> Input::keys_released;
std::vector<int> Input::keys_pressed;


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

void Input::ClearBuffer()
{
	keys_pressed.clear();
	keys_released.clear();
}
