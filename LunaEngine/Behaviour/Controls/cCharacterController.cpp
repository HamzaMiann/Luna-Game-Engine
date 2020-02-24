#include "cCharacterController.h"
#include <InputManager.h>
#include <xml_helper.h>
#include <Camera.h>
#include <Physics/Mathf.h>
#include <DebugRenderer/cDebugRenderer.h>
using namespace rapidxml;

bool cCharacterController::deserialize(xml_node<>* root_node)
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

void cCharacterController::start()
{
	anim = parent.GetComponent<cAnimationController>();
	rigidBody = parent.GetComponent<nPhysics::iPhysicsComponent>();
	rotX = quat(vec3(0.f));
	rotY = quat(vec3(0.f));
	jumpVelocity = vec3(0.f);
	Input::LockCursor();
}

void cCharacterController::update(float dt)
{
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

	previousX = x;
	previousY = y;

	if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_1))
	{
		settings.distance_from_object += dt * 10.f;
	}
	if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_2))
	{
		settings.distance_from_object -= dt * 10.f;
	}

	direction = settings.forward * rotX;
	direction.y = (settings.forward * rotY).y;
	direction = glm::normalize(direction);

	Camera::main_camera->Eye = transform.Position()
		+ (settings.camera_offset * rotX)
		+ (direction * settings.distance_from_object);
	Camera::main_camera->Target = transform.Position()
		+ (settings.camera_offset * rotX)
		+ direction;

	if (anim && rigidBody)
	{
		direction.y = 0.f;
		direction = normalize(direction);

		quat rotation = Mathf::RotationFromTo(settings.forward, direction);
		vec3 movement(0.f);

		direction *= -1.f;

		if (isJumping || isAttacking)
		{
			float t = anim->GetCurrentTime();
			if (t > animationDuration)
			{
				isJumping = false;
				isAttacking = false;
			}
		}
		else
		{
			if (Input::KeyDown(GLFW_KEY_SPACE))
			{
				anim->SetAnimation("jump", 0.f);
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
					anim->SetAnimation("run-jump", 0.f);
				}
				else
					jumpVelocity = vec3(0.f);
				isJumping = true;
				animationDuration = anim->GetAnimationDuration();

				vec3 velocity = rigidBody->GetVelocity();
				jumpVelocity *= settings.speed;
				jumpVelocity.y = velocity.y + settings.speed;
				rigidBody->SetVelocity(jumpVelocity);

				return;
			}
			else if (Input::KeyDown(GLFW_KEY_F))
			{
				anim->SetAnimation("attack", 0.f);
				isAttacking = true;
				animationDuration = anim->GetAnimationDuration();
				jumpVelocity = vec3(0.f);
				vec3 velocity = rigidBody->GetVelocity();
				jumpVelocity.y = velocity.y;
				rigidBody->SetVelocity(jumpVelocity);
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
					anim->SetAnimation("run");
				}
				else
				{
					anim->SetAnimation("walk");
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
				anim->SetAnimation("walk-backwards");
			}
			else if (Input::GetKey(GLFW_KEY_A))
			{
				movement = direction * quat(vec3(0.f, glm::radians(-90.f), 0.f));
				transform.Rotation(glm::slerp(transform.Rotation(), rotation, dt * 10.f));
				anim->SetAnimation("walk-left");
			}
			else if (Input::GetKey(GLFW_KEY_D))
			{
				movement = direction * quat(vec3(0.f, glm::radians(90.f), 0.f));;
				transform.Rotation(glm::slerp(transform.Rotation(), rotation, dt * 10.f));
				anim->SetAnimation("walk-right");
			}
			else
			{
				anim->SetAnimation("idle");
			}

			vec3 velocity = rigidBody->GetVelocity();
			movement *= settings.speed;
			movement.y = velocity.y;
			rigidBody->SetVelocity(movement);
			
		}
	}

}

void cCharacterController::OnDestroy()
{
	if (anim)
	{
		anim->SetAnimation("idle");
	}
	Input::UnlockCursor();
}

void cCharacterController::SetSettings(const sCharacterControllerSettings& settings)
{
	this->settings = settings;
}
