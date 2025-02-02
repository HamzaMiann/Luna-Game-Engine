#include "cFPSController.h"
#include <Camera.h>
#include <InputManager.h>
#include <xml_helper.h>
#include <Physics\Mathf.h>
#include <EntityManager\cEntityManager.h>
#include <Physics/global_physics.h>
#include <cGameObject.h>
#include <Shader/Shader.h>
#include <Rendering/RenderingEngine.h>
#include <Audio\AudioEngine.hpp>
#include <Lighting/cLightManager.h>
#include <Behaviour/AI/cZombie.h>
#include "cLifeTimer.h"
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

vec3 actualOffset;
vec3 targetOffset;
vec3 offset2;
float time;
float sinY;
float blendRatio = 0.f;
float totalY = 0.f;
float totalX = 0.f;
void cFPSController::start()
{
	rigidBody = parent.GetComponent<nPhysics::iCharacterComponent>();
	rigidBody->SetMaxSlope(glm::radians(40.f));
	rigidBody->SetStepHeight(1.f);

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

bool cFPSController::CalculateRotation()
{
	double x, y;
	Input::CursorPosition(x, y);
	if (previousX == 0. && previousY == 0.)
	{
		previousX = x;
		previousY = y;
		return false;
	}

	float deltaX = glm::radians(x - previousX);
	float deltaY = glm::radians(y - previousY);

	totalX += deltaX * mDt * settings.mouse_speed;
	totalY += deltaY * mDt * settings.mouse_speed;

	totalY = glm::clamp(totalY, glm::radians(-55.f), glm::radians(55.f));

	rotX = quat(vec3(0.f, totalX, 0.f));
	rotY = quat(vec3(-totalY, 0.f, 0.f));
	rotYi = quat(vec3(totalY, 0.f, 0.f));

	previousX = x;
	previousY = y;

	return true;
}

float GetVerticalAxis()
{
	float input = 0.f;
	input += (float)Input::GetKey(GLFW_KEY_W);
	input -= (float)Input::GetKey(GLFW_KEY_S);
	return input;
}

float GetHorizontalAxis()
{
	float input = 0.f;
	input += (float)Input::GetKey(GLFW_KEY_A);
	input -= (float)Input::GetKey(GLFW_KEY_D);
	return input;
}

cGameObject* MakeHitPoint(const vec3& position)
{
	auto obj = new cGameObject;
	obj->transform.Position(position);
	obj->transform.Scale(vec3(0.1f));
	obj->meshName = "sphere";
	obj->shader = Shader::FromName("basic");
	obj->texture[0].SetTexture("bark_0021.jpg", 1.f);
	return obj;
}

void cFPSController::update(float dt)
{
	mDt = dt;
	time += dt;

	if (!CalculateRotation()) return;
	

	if (Input::GetKey(GLFW_KEY_E))
	{
		blendRatio = Mathf::lerp(blendRatio, 1., dt * 3.f);
	}
	else
	{
		blendRatio = Mathf::lerp(blendRatio, 0., dt * 3.f);
	}
	
	RenderingEngine::Instance().SetProperty("blendRatio", blendRatio);

	cLightManager::Instance()->Lights[2]->param2.x = 0.f;
	if (Input::MouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		Shoot();
	}

	
	// SCROLL ZOOM
	settings.distance_from_object += Input::GetScrollY() * -1.f;
	if (settings.distance_from_object < 2.f)
		settings.distance_from_object = 2.f;

	direction = settings.forward * rotX;
	direction.y = (settings.forward * rotY).y;
	direction = glm::normalize(direction);

	if (Input::GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
	{
		offset2.y = settings.camera_offset.y;
		targetOffset = offset2;
	}
	else
	{
		targetOffset = settings.camera_offset;
	}

	
	float timeSpeed = 8.f;
	if (Input::GetKey(GLFW_KEY_LEFT_SHIFT))
	{
		timeSpeed += 4.f;
	}

	sinY = settings.camera_offset.y + sin(time * timeSpeed) / 5.f;

	

	if (Input::GetKey(GLFW_KEY_W) || Input::GetKey(GLFW_KEY_A) || Input::GetKey(GLFW_KEY_D) || Input::GetKey(GLFW_KEY_S))
	{
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

	if (weapon) {
		vec3 pos = vec3(0.f, 4.45f, 0.f);
		pos = transform.rotation * pos + transform.Position();
		vec3 lerp = Mathf::lerp(weapon->transform.pos, pos, dt * 10.f);
		weapon->transform.Position(lerp);

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
	HandleMovement(direction);

}

void cFPSController::OnCollide(iObject* other)
{
}

void cFPSController::OnDestroy()
{
	Input::UnlockCursor();
}

void cFPSController::HandleMovement(vec3 direction)
{
	vec3 right = quat(vec3(0.f, glm::radians(90.f), 0.f)) * direction;

	if (rigidBody)
	{
		vec3 walkDirection(0.f);
		float vertical = GetVerticalAxis();
		float horizontal = GetHorizontalAxis();

		walkDirection += direction * vertical + (right * horizontal);
		if (walkDirection != vec3(0.f))
			walkDirection = normalize(walkDirection);

		if (Input::GetKey(GLFW_KEY_LEFT_SHIFT) && vertical > 0.f)
		{
			walkDirection *= 2.f;
		}

		rigidBody->SetWalkDirection(walkDirection);

		rigidBody->Walk(settings.speed * mDt);

		if (Input::KeyDown(GLFW_KEY_SPACE) && rigidBody->CanJump())
		{
			rigidBody->Jump(vec3(0.f, 10.f, 0.f));
		}
	}

	cLightManager::Instance()->Lights[2]->position = vec4(transform.Position(), 1.f);

}

void cFPSController::Shoot()
{
	AudioEngine::Instance()->PlaySound("gunshot");
	cLightManager::Instance()->Lights[2]->param2.x = 1.f;

	vec3 ro = Camera::main_camera->Eye;
	vec3 rd = glm::normalize(Camera::main_camera->Target - Camera::main_camera->Eye);
	rd = weapon->transform.Rotation() * vec3(0.f, 0.f, 1.f);
	auto* hit = g_PhysicsWorld->RayCast(ro, rd, 300.f, rigidBody);
	if (hit)
	{
		hit->object->AddForceToPoint(rd * 100.f, hit->hitPoint);
		cGameObject* obj = MakeHitPoint(hit->hitPoint);
		obj->AddComponent<cLifeTimer>()->mTime = 3.f;
		cEntityManager::Instance().AddEntity(obj);

		AI::cZombie* component = hit->object->parent.GetComponent<AI::cZombie>();
		if (component)
		{
			component->TakeDamage(10.f);
		}
	}

	totalX += glm::radians(Mathf::randInRange(-5.f, 5.f));
	totalY += glm::radians(Mathf::randInRange(-10.f, -7.f));
}
