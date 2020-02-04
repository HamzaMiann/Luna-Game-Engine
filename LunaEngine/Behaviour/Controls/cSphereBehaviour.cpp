#include "cSphereBehaviour.h"
#include <_GL/Window.h>
#include <Camera.h>
#include <Physics/Mathf.h>

typedef glm::vec3 vec3;
typedef glm::quat quat;

vec3 cSphereBehaviour::direction = vec3(0.f, 0.f, 1.f);
float cSphereBehaviour::distance_from_object = 10.f;

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
	body = parent.GetComponent<nPhysics::iPhysicsComponent>();
	rotX = quat(vec3(0.f, 0.f, 0.f));
	rotY = quat(vec3(0.f, 0.f, 0.f));
	camera_target = Camera::main_camera->Target;
	camera_position = Camera::main_camera->Eye;
}

void cSphereBehaviour::update(float dt)
{
	camera_target = transform.pos;// Mathf::lerp(camera_target, transform.pos, dt * 5.f);
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

	rotX *= quat(vec3(
		0.f,//glm::radians(y - previousY) * dt,
		glm::radians(x - previousX) * dt,
		0.f
	) * rotation_speed);

	rotY *= quat(vec3(
		glm::radians(y - previousY) * dt,
		0.f,//glm::radians(x - previousX) * dt,
		0.f
	) * rotation_speed);

	//rotation *= glm::quat(eulerRotation);

	vec3 dirY = direction * rotY;
	vec3 newDir = direction * rotX;
	newDir.y = dirY.y;

	auto pos = transform.pos + (distance_from_object * newDir) + vec3(0.f, 3.f, 0.f);

	float previousCameraY = camera_position.y;
	camera_position = pos;
	//camera_position.y = Mathf::lerp(previousCameraY, pos.y, dt);

	Camera::main_camera->Eye = camera_position;

	if (body)
	{
		auto force_direction = -newDir;
		force_direction.y = 0.f;
		force_direction = glm::normalize(force_direction);

		if (glfwGetKey(global::window, GLFW_KEY_W))
		{
			body->AddForce(force_direction * 5.f);
		}

		if (glfwGetKey(global::window, GLFW_KEY_S))
		{
			body->AddForce(force_direction * -5.f);
		}

		if (glfwGetKey(global::window, GLFW_KEY_D))
		{
			body->AddForce((force_direction * -5.f) * quat(vec3(0.f, glm::radians(-90.f), 0.f)));
		}

		if (glfwGetKey(global::window, GLFW_KEY_A))
		{
			body->AddForce((force_direction * -5.f) * quat(vec3(0.f, glm::radians(90.f), 0.f)));
		}

		if (glfwGetKey(global::window, GLFW_KEY_SPACE))
		{
			body->AddForce(glm::vec3(0.f, 1.f, 0.f) * 100.f);
		}

		if (glfwGetMouseButton(global::window, GLFW_MOUSE_BUTTON_1))
		{
			distance_from_object *= 1.01f;
		}

		if (glfwGetMouseButton(global::window, GLFW_MOUSE_BUTTON_2))
		{
			distance_from_object *= 0.99f;
		}
		
	}

	previousX = x;
	previousY = y;
}
