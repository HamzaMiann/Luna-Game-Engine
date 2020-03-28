#include "cFPSController.h"
#include <Camera.h>
#include <InputManager.h>
#include <xml_helper.h>
#include <Physics\Mathf.h>
#include <EntityManager\cEntityManager.h>
using namespace rapidxml;

bool cFPSController::deserialize(rapidxml::xml_node<>* root_node)
{
	for (xml_node<>* node = root_node->first_node(); node; node = node->next_sibling())
	{
		std::string name = node->name();
		if (name == "Forward")
		{
			settings.forward = XML_Helper::AsVec3(node);
		}
		if (name == "Offset")
		{
			settings.camera_offset = XML_Helper::AsVec3(node);
		}
		if (name == "CameraDistance")
		{
			settings.distance_from_object = XML_Helper::AsFloat(node);
		}
		if (name == "Speed")
		{
			settings.speed = XML_Helper::AsFloat(node);
		}
		if (name == "MouseSpeed")
		{
			settings.mouse_speed = XML_Helper::AsFloat(node);
		}
	}
	return true;
}

quat rotYi;
vec3 actualOffset;
vec3 targetOffset;
vec3 offset2;
float time;
float sinY;
void cFPSController::start()
{
	rigidBody = parent.GetComponent<nPhysics::iPhysicsComponent>();
	rotX = quat(vec3(0.f));
	rotY = quat(vec3(0.f));
	rotYi = quat(vec3(0.f));
	jumpVelocity = vec3(0.f);
	actualOffset = settings.camera_offset;
	targetOffset = settings.camera_offset;
	offset2 = settings.camera_offset;
	offset2.x = 0.f;
	offset2.z = 0.f;
	sinY = settings.camera_offset.y;
	weapon = cEntityManager::Instance().GetObjectByTag("gun");
	Input::LockCursor();
}

void cFPSController::update(float dt)
{
	time += dt;

	double x, y;
	Input::CursorPosition(x, y);
	if (previousX == 0. && previousY == 0.)
	{
		previousX = x;
		previousY = y;
		return;
	}

	rotX *= quat(vec3(
		0.f,
		(x - previousX) * dt * settings.mouse_speed,
		0.f
		));

	rotY *= quat(vec3(
	(y - previousY) * -dt * settings.mouse_speed,
		0.f,
		0.f
		));

	rotYi *= quat(vec3(
	(y - previousY) * dt * settings.mouse_speed,
		0.f,
		0.f
		));

	previousX = x;
	previousY = y;

	
	// SCROLL ZOOM
	settings.distance_from_object += Input::GetScrollY() * -1.f;
	if (settings.distance_from_object < 2.f)
		settings.distance_from_object = 2.f;

	direction = settings.forward * rotX;
	direction.y = (settings.forward * rotY).y;
	direction = glm::normalize(direction);

	if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_2))
	{
		offset2.y = settings.camera_offset.y;
		targetOffset = offset2;
	}
	else {
		targetOffset = settings.camera_offset;
	}

	float timeSpeed = 8.f;
	if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
	{
		timeSpeed += 4.f;
	}

	sinY = settings.camera_offset.y + sin(time * timeSpeed) / 15.f;

	

	if (Input::GetKey(GLFW_KEY_W) || Input::GetKey(GLFW_KEY_A) || Input::GetKey(GLFW_KEY_D) || Input::GetKey(GLFW_KEY_S)) {
		targetOffset.y = sinY;
	}

	actualOffset = Mathf::lerp(actualOffset, targetOffset, dt * 10.f);

	Camera::main_camera->Eye = transform.Position()
		+ (actualOffset * rotX)
		+ (direction * settings.distance_from_object);

	Camera::main_camera->Target = transform.Position()
		+ (actualOffset * rotX)
		+ direction;

	

	direction.y = 0.f;
	direction = normalize(direction);
	quat rotation = Mathf::RotationFromTo(settings.forward, direction);
	vec3 movement(0.f);

	if (weapon) {
		weapon->transform.Position(transform.Position());
		weapon->transform.pos.y += 4.f;
		quat slerp = glm::slerp(weapon->transform.Rotation(),
			Mathf::RotationFromTo(
				settings.forward,
				direction
				) * rotYi,
				dt * 10.f
			);
		weapon->transform.Rotation(slerp);
	}

	direction *= -1.f;

	if (rigidBody)
	{

		if (Input::KeyDown(GLFW_KEY_SPACE))
		{
			if (Input::GetKey(GLFW_KEY_W))
			{
				if (Input::GetKey(GLFW_KEY_A))
				{
					direction = direction * quat(vec3(0.f, glm::radians(-45.f), 0.f));
				}
				else if (Input::GetKey(GLFW_KEY_D))
				{
					direction = direction * quat(vec3(0.f, glm::radians(45.f), 0.f));
				}
				if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
				{
					direction *= 3.f;
				}
				jumpVelocity = direction;
			}
			else
				jumpVelocity = vec3(0.f);

			vec3 velocity = rigidBody->GetVelocity();
			jumpVelocity *= settings.speed;


			rigidBody->SetVelocity(jumpVelocity);
			return;
		}
		else if (Input::GetKey(GLFW_KEY_W))
		{
			if (Input::GetKey(GLFW_KEY_A))
			{
				direction = direction * quat(vec3(0.f, glm::radians(-45.f), 0.f));
				rotation = Mathf::RotationFromTo(settings.forward, -direction);
			}
			else if (Input::GetKey(GLFW_KEY_D))
			{
				direction = direction * quat(vec3(0.f, glm::radians(45.f), 0.f));
				rotation = Mathf::RotationFromTo(settings.forward, -direction);
			}

			if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
			{
				direction *= 3.f;
			}

			transform.Rotation(glm::slerp(transform.Rotation(), rotation, dt * 10.f));
			movement = direction;
		}
		else if (Input::GetKey(GLFW_KEY_S))
		{
			if (Input::GetKey(GLFW_KEY_A))
			{
				direction = direction * quat(vec3(0.f, glm::radians(45.f), 0.f));
				rotation = Mathf::RotationFromTo(settings.forward, -direction);
			}
			else if (Input::GetKey(GLFW_KEY_D))
			{
				direction = direction * quat(vec3(0.f, glm::radians(-45.f), 0.f));
				rotation = Mathf::RotationFromTo(settings.forward, -direction);
			}
			movement = -direction;
			transform.Rotation(glm::slerp(transform.Rotation(), rotation, dt * 10.f));
		}
		else if (Input::GetKey(GLFW_KEY_A))
		{
			movement = direction * quat(vec3(0.f, glm::radians(-90.f), 0.f));
			transform.Rotation(glm::slerp(transform.Rotation(), rotation, dt * 10.f));
		}
		else if (Input::GetKey(GLFW_KEY_D))
		{
			movement = direction * quat(vec3(0.f, glm::radians(90.f), 0.f));;
			transform.Rotation(glm::slerp(transform.Rotation(), rotation, dt * 10.f));
		}
		else
		{
		}

		vec3 velocity = rigidBody->GetVelocity();
		movement *= settings.speed;
		movement.y = velocity.y;
		rigidBody->SetVelocity(movement);
		
	}
}

void cFPSController::OnDestroy()
{
	Input::UnlockCursor();
}
