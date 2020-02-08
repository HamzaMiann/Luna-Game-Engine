#include "cFPSController.h"
#include <Camera.h>
#include <_GL/Window.h>

void cFPSController::start()
{
	rotX = quat(vec3(0.f));
	direction = vec3(0.f, 0.f, -1.f);
	maxAngleY = 60.f;
	angleY = 0.f;
	position_offset = vec3(-0.5f, 0.7f, 4.f);
	speed = 2.f;
	glfwSetInputMode(global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void cFPSController::update(float dt)
{
	double x, y;
	glfwGetCursorPos(global::window, &x, &y);

	if (previousX == 0 && previousY == 0)
	{
		previousX = x;
		previousY = y;
		return;
	}

	quat rotation = quat(
		vec3(
			0.f,
			(x - previousX) * dt * -0.1f,
			0.f
		)
	);

	quat right = quat(vec3(0.f, 1.f, 0.f));

	rotX *= rotation;

	vec3 actual_dir = rotX * direction;

	if (glfwGetKey(global::window, GLFW_KEY_W))
	{
		transform.pos += actual_dir * dt * speed;
	}
	else if (glfwGetKey(global::window, GLFW_KEY_S))
	{
		transform.pos += actual_dir * dt * -speed;
	}

	if (glfwGetKey(global::window, GLFW_KEY_D))
	{
		transform.pos += actual_dir * right * dt * speed;
	}
	else if (glfwGetKey(global::window, GLFW_KEY_A))
	{
		transform.pos += actual_dir * right * dt * -speed;
	}

	Camera::main_camera->Eye = transform.pos + position_offset;
	Camera::main_camera->Target = Camera::main_camera->Eye + actual_dir;

	previousX = x;
	previousY = y;
}
