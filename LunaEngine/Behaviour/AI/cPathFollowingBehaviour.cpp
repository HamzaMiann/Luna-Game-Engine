#include "cPathFollowingBehaviour.h"
#include <InputManager.h>
#include <Behaviour/Managers/cPathFindingManager.h>
#include <EntityManager/cEntityManager.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <cGameObject.h>
#include <Shader/Shader.h>
#include <Physics/Mathf.h>

namespace AI {

	void cPathFollowingBehaviour::start()
	{
		state = STATE::WAITING_FOR_KEY;
		manager = cEntityManager::Instance().GetObjectByTag("manager")->GetComponent<cPathFindingManager>();
		anim = parent.GetComponent<cAnimationController>();
	}

	void cPathFollowingBehaviour::update(float dt)
	{
		if (state == STATE::WAITING_FOR_KEY)
		{
			if (Input::KeyUp(GLFW_KEY_ENTER))
			{
				this->FindResource();
			}
		}
		else if (state == STATE::GOING_TO_RESOURCE)
		{
			anim->SetAnimation("walk");
			this->FollowPath(dt);
		}
		else if (state == STATE::COLLECTING_RESOURCE)
		{
			anim->SetAnimation("attack");
			this->CollectResource(dt);
		}
		else if (state == STATE::GOING_HOME)
		{
			anim->SetAnimation("walk");
			this->FollowPath(dt);
		}
		else if (state == STATE::DELIVERING)
		{
			anim->SetAnimation("attack");
			this->Deliver(dt);
		}
		else
		{
			anim->SetAnimation("idle");
		}

		if (resource)
		{
			resource->transform.Position(transform.Position() + vec3(1.f, 1.f, 0.f));
		}
	}

	void cPathFollowingBehaviour::FindResource()
	{
		path = manager->GetPathToClosestResource(current);
		if (path.size() > 0)
		{
			current = path[0];
			state = STATE::GOING_TO_RESOURCE;
		}
		else {
			state = STATE::NONE;
		}
	}

	void cPathFollowingBehaviour::FollowPath(float dt)
	{
		for (unsigned int i = 0; i < path.size() - 1; ++i)
		{
			cDebugRenderer::Instance().addLine(path[i]->position + vec3(0.f, 1.f, 0.f), path[i + 1]->position + vec3(0.f, 1.f, 0.f), vec3(1.f, 0.f, 0.f), dt);
		}
		if (glm::distance(parent.transform.Position(), (path[0]->position + vec3(0.f, 1.f, 0.f))) < 0.1f)
		{
			current = path[0];
			path.erase(path.begin());
			if (path.size() == 0)
			{
				if (state == STATE::GOING_TO_RESOURCE)
				{
					state = STATE::COLLECTING_RESOURCE;
				}
				else
				{
					state = STATE::DELIVERING;
				}
				deliverTimer = 2.f;
				resourceTimer = 7.f;
			}
		}
		else
		{
			vec3 dir = glm::normalize(path[0]->position + vec3(0.f, 1.f, 0.f) - parent.transform.Position());
			parent.transform.pos += dir * dt * 2.f;
			transform.Rotation(Mathf::RotationFromTo(vec3(0.f, 0.f, 1.f), dir));
		}
	}

	void cPathFollowingBehaviour::CollectResource(float dt)
	{
		if (resourceTimer > 0.f)
		{
			resourceTimer -= dt;
		}
		else
		{
			cGameObject* obj = new cGameObject;
			obj->shader = Shader::FromName("basic");
			obj->meshName = "sphere";
			obj->texture[0].SetTexture("blue.png", 1.f);
			obj->transform.Scale(vec3(0.3f));
			resource = obj;
			cEntityManager::Instance().AddEntity(obj);

			path = manager->GetPathToHome(current);
			state = STATE::GOING_HOME;
		}
	}

	void cPathFollowingBehaviour::Deliver(float dt)
	{
		if (deliverTimer > 0.f)
		{
			deliverTimer -= dt;
		}
		else
		{
			cEntityManager::Instance().RemoveEntity(dynamic_cast<cGameObject*>(resource));
			resource = 0;
			FindResource();
		}
	}

}