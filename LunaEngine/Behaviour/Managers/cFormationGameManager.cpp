#include "cFormationGameManager.h"
#include <EntityManager/cEntityManager.h>
#include <cGameObject.h>
#include <InputManager.h>
#include <Physics/Mathf.h>

void cFormationGameManager::start()
{
	maxVelocity = 10.f;

	auto& objects = cEntityManager::Instance().GetEntities();
	for (cGameObject* obj : objects)
	{
		if (obj->tag != "agent") continue;

		AI::cFormationBehaviour* behaviour = obj->AddComponent<AI::cFormationBehaviour>();
		behaviour->manager = this;
		agents.push_back(behaviour);
		positions.push_back(transform.Position());
	}
	state = AI::AI_STATE::NONE;
}

void cFormationGameManager::update(float dt)
{
	HandleInput();
	HandleState();
}

void cFormationGameManager::HandleInput()
{
	if (Input::KeyUp(GLFW_KEY_1))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::Circle;
		NotifyStateChange();
	}
	else if (Input::KeyUp(GLFW_KEY_2))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::V;
		NotifyStateChange();
	}
	else if (Input::KeyUp(GLFW_KEY_3))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::V;
		NotifyStateChange();
	}
	else if (Input::KeyUp(GLFW_KEY_4))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::V;
		NotifyStateChange();
	}
	else if (Input::KeyUp(GLFW_KEY_5))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::V;
		NotifyStateChange();
	}
}

void cFormationGameManager::HandleState()
{
	if (state == AI::AI_STATE::NONE)
	{
		state = AI::AI_STATE::FORMATION;
		NotifyStateChange();
	}
	else if (state == AI::AI_STATE::FORMATION)
	{
		switch (formation)
		{
		case FORM::Circle: SetCircle(); break;
		case FORM::V: SetV(); break;
		case FORM::Square: SetSquare(); break;
		case FORM::Line: SetLine(); break;
		case FORM::TwoRows: SetRows(); break;
		default: break;
		}
	}
	else if (state == AI::AI_STATE::FLOCKING)
	{
		// TODO
	}
}

void cFormationGameManager::NotifyStateChange()
{
	for (AI::cFormationBehaviour* agent : agents)
	{
		agent->state = state;
	}
}

void cFormationGameManager::SetCircle()
{
	vec3 pos = transform.Position();
	float angle = 0.f;
	float radius = 7.f;
	float step = 360.f / (float)agents.size();
	for (unsigned int i = 0; i < agents.size(); ++i)
	{
		float rad = glm::radians(angle);
		float x = Mathf::Sin(rad) * radius;
		float z = Mathf::Cos(rad) * radius;
		positions[i].x = pos.x + x;
		positions[i].z = pos.z + z;
		positions[i].y = pos.y;
		agents[i]->target = positions[i];
		angle += step;
	}
}

void cFormationGameManager::SetV()
{
	vec3 pos = transform.Position();
	float dist = 1.f;
	float spread = 1.f;
	float step = 2.f;
	for (unsigned int i = 0; i+1u < agents.size(); i += 2)
	{
		float x = dist;
		float x2 = -dist;
		positions[i] = vec3(pos.x + x, pos.y, pos.z);
		positions[i+1u] = vec3(pos.x + x2, pos.y, pos.z);

		agents[i]->target = positions[i];
		agents[i+1u]->target = positions[i+1u];

		dist += spread;
		pos.z += step;
	}

	if (agents.size() % 2 == 1)
	{
		// TODO
	}
}

void cFormationGameManager::SetSquare()
{
	int numPerSide = (int)ceil(sqrt(agents.size()));
}

void cFormationGameManager::SetLine()
{
	// TODO
}

void cFormationGameManager::SetRows()
{
	// TODO
}
