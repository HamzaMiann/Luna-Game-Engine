#include "cPlayerBehaviour.h"
#include <_GL/Window.h>
#include <Camera.h>
#include <EntityManager/cEntityManager.h>
#include <Behaviour/Managers/cAIGameManager.h>

#pragma warning(disable)


cPlayerBehaviour::cPlayerBehaviour(iObject* root)
	: iBehaviour(root)
{
}

cPlayerBehaviour::~cPlayerBehaviour()
{
}

bool cPlayerBehaviour::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cPlayerBehaviour::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

void cPlayerBehaviour::start()
{
	rb = parent.GetComponent<nPhysics::iPhysicsComponent>();
	start_pos = transform.pos;
	reload_time = -1.f;
}

void cPlayerBehaviour::update(float dt)
{

	if (rb)
	{
		float speed = 3.f;

		direction = transform.rotation * forward;

		if (glfwGetKey(global::window, GLFW_KEY_W))
		{
			rb->SetVelocity(rb->GetVelocity() + direction * speed * dt);
		}
		if (glfwGetKey(global::window, GLFW_KEY_S))
		{
			rb->SetVelocity(rb->GetVelocity() - direction * speed * dt);
		}
		if (glfwGetKey(global::window, GLFW_KEY_A))
		{
			transform.UpdateEulerRotation(vec3(0.f, dt * 90.f, 0.f));
		}
		if (glfwGetKey(global::window, GLFW_KEY_D))
		{
			transform.UpdateEulerRotation(vec3(0.f, -dt * 90.f, 0.f));
		}

		if (glfwGetKey(global::window, GLFW_KEY_SPACE) && reload_time < 0.f)
		{
			reload_time = 3.f;
			iObject* obj = (iObject*)cEntityManager::Instance().GetObjectByTag("manager");
			cAIGameManager* manager = obj->GetComponent<cAIGameManager>();
			if (manager)
			{
				manager->Player_Shoot(transform.pos + direction * 1.5f, direction * 7.f);
			}
		}

	}

	reload_time -= dt;

	Camera::main_camera->Eye = transform.pos;
	transform.pos.z += 0.01f;
	Camera::main_camera->Eye.y += 40.f;
	Camera::main_camera->Target = transform.pos;
}

void cPlayerBehaviour::OnCollide(iObject* other)
{
	if (other->tag == "enemy")
	{
		cEntityManager::Instance().RemoveEntity(other);
		Reset();
	}
}

void cPlayerBehaviour::Reset()
{
	transform.pos = start_pos;
	rb->SetPosition(transform.pos);
	rb->SetVelocity(vec3(0.f));
}
