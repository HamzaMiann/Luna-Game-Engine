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
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_2))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::V;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_3))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::Square;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_4))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::Line;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_5))
	{
		state = AI::AI_STATE::FORMATION;
		formation = FORM::TwoRows;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_6))
	{
		// TODO
	}
	else if (Input::KeyUp(GLFW_KEY_7))
	{
		state = AI::AI_STATE::FORMATION;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_8))
	{
		// TODO
	}
	else if (Input::KeyUp(GLFW_KEY_9))
	{
		// TODO
	}
	else if (Input::KeyUp(GLFW_KEY_0))
	{
		// TODO
	}
}

void cFormationGameManager::HandleState()
{
	if (state == AI::AI_STATE::NONE)
	{
		state = AI::AI_STATE::FORMATION;
		NotifyStateChange(state);
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

void cFormationGameManager::NotifyStateChange(AI::AI_STATE _state)
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
	vec3 pos = transform.Position();

	float dist= 3.5f;

	agents[0u]->target = vec3(pos.x + 0.f * dist, pos.y, pos.z + 1.f * dist);
	agents[1u]->target = vec3(pos.x + -1.f * dist, pos.y, pos.z + 1.f * dist);
	agents[2u]->target = vec3(pos.x + 1.f * dist, pos.y, pos.z + 1.f * dist);
	agents[3u]->target = vec3(pos.x + 2.f * dist, pos.y, pos.z + 1.f * dist);

	agents[4u]->target = vec3(pos.x + 2.f * dist, pos.y, pos.z + 0.f * dist);
	agents[5u]->target = vec3(pos.x + 2.f * dist, pos.y, pos.z + -1.f * dist);
	agents[6u]->target = vec3(pos.x + 2.f * dist, pos.y, pos.z + -2.f * dist);
	agents[7u]->target = vec3(pos.x + 1.f * dist, pos.y, pos.z + -2.f * dist);

	agents[8u]->target = vec3(pos.x + 0.f * dist, pos.y, pos.z + -2.f * dist);
	agents[9u]->target = vec3(pos.x + -1.f * dist, pos.y, pos.z + -2.f * dist);
	agents[10u]->target = vec3(pos.x + -1.f * dist, pos.y, pos.z + -1.f * dist);
	agents[11u]->target = vec3(pos.x + -1.f * dist, pos.y, pos.z + 0.f * dist);
}

void cFormationGameManager::SetLine()
{
	vec3 pos = transform.Position();

	float dist = 3.0f;

	char index = 0;
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
	agents[index]->target = vec3(pos.x + (-6.f + index++) * dist, pos.y, pos.z);
}

void cFormationGameManager::SetRows()
{
	vec3 pos = transform.Position();

	float dist = 3.0f;
	char index = 0;

	agents[0u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z);
	agents[1u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z);
	agents[2u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z);
	agents[3u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z);
	agents[4u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z);
	agents[5u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z);

	index = 0;
	agents[6u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z + dist);
	agents[7u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z + dist);
	agents[8u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z + dist);
	agents[9u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z + dist);
	agents[10u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z + dist);
	agents[11u]->target = vec3(pos.x + (-3.f + index++) * dist, pos.y, pos.z + dist);
}
