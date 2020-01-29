#include "cSphereBehaviour.h"
#include <Components/cRigidBody.h>
#include <_GL/Window.h>
#include <Camera.h>
#include <Physics/Mathf.h>

cSphereBehaviour::cSphereBehaviour(iObject* root)
	: iBehaviour(root)
	, body(nullptr)
{
	glfwSetInputMode(global::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

cSphereBehaviour::~cSphereBehaviour()
{
	glfwSetInputMode(global::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool cSphereBehaviour::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cSphereBehaviour::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

void cSphereBehaviour::start()
{
	body = parent.GetComponent<cRigidBody>();
	rotation = glm::quat(glm::vec3(0.f, 0.f, 0.f));
	distance_from_object = 5.f;
	camera_target = Camera::main_camera->Target;
	camera_position = Camera::main_camera->Eye;
}

void cSphereBehaviour::update(float dt)
{
	if (body)
	{
		camera_target = Mathf::lerp(camera_target, transform.pos, dt);
		Camera::main_camera->Target = camera_target;

		double x, y;
		glfwGetCursorPos(global::window, &x, &y);

		if (previousX == 0 && previousY == 0)
		{
			previousX = x;
			previousY = y;
			return;
		}
		
		float rotation_speed = 2.f;

		glm::vec3 eulerRotation = glm::vec3(
			0.f,//glm::radians(y - previousY) * dt,
			glm::radians(x - previousX) * dt,
			0.f
		) * rotation_speed;

		//rotation *= glm::quat(eulerRotation);

		direction = direction * glm::quat(eulerRotation);//rotation;

		auto pos = transform.pos + (distance_from_object * direction) + glm::vec3(0.f, 3.f, 0.f);

		float previousCameraY = camera_position.y;
		camera_position = pos;
		camera_position.y = Mathf::lerp(previousCameraY, pos.y, dt);

		Camera::main_camera->Eye = camera_position;

		auto force_direction = -direction;
		force_direction.y = 0.f;
		force_direction = glm::normalize(force_direction);

		if (glfwGetKey(global::window, GLFW_KEY_W))
		{
			body->AddForce(force_direction * 5.f);
			//printf("PRESSED");
		}

		if (glfwGetKey(global::window, GLFW_KEY_SPACE))
		{
			body->AddForce(glm::vec3(0.f, 1.f, 0.f) * 100.f);
			//printf("PRESSED");
		}

		previousX = x;
		previousY = y;


	}
}
