#include "cFormationGameManager.h"
#include <EntityManager/cEntityManager.h>
#include <cGameObject.h>
#include <InputManager.h>
#include <Physics/Mathf.h>
#include <iostream>
#include <iomanip>
#include <DebugRenderer/cDebugRenderer.h>

void cFormationGameManager::start()
{
	maxVelocity = 5.f;
	separationRadius = 15.f;
	alignmentRadius = 30.f;
	cohesionRadius = 300.f;

	weight.cohesion = 0.7f;
	weight.alignment = 0.15f;
	weight.separation = 0.15f;

	velocity = vec3(0.f);
	screenLocked = true;

	auto& objects = cEntityManager::Instance().GetEntities();
	for (cGameObject* obj : objects)
	{
		if (obj->tag == "agent")
		{
			AI::cFormationBehaviour* behaviour = obj->AddComponent<AI::cFormationBehaviour>();
			behaviour->manager = this;
			agents.push_back(behaviour);
			positions.push_back(transform.Position());
		}
		if (obj->tag == "node")
		{
			nodes.push_back(obj);
		}
	}
	state = AI::AI_STATE::NONE;
}

void cFormationGameManager::update(float dt)
{
	HandleInput(dt);
	HandleState(dt);
}

void cFormationGameManager::IncreaseWeight(float& increased, float& decreased1, float& decreased2, float dt)
{
	if (increased == 1.f) return;

	increased += dt;

	decreased1 -= dt / 2.f;
	decreased2 -= dt / 2.f;

	if (increased > 1.f)
	{
		increased = 1.f;
		decreased1 = 0.f;
		decreased2 = 0.f;
	}
	if (decreased1 < 0.f)
	{
		decreased1 = 0.f;
		decreased2 = (1.f - increased);
	}
	if (decreased2 < 0.f)
	{
		decreased2 = 0.f;
		decreased1 = (1.f - increased);
	}

}

void cFormationGameManager::DecreaseWeight(float& decreased, float& increased1, float& increased2, float dt)
{
	if (decreased == 0.f) return;

	decreased -= dt;

	increased1 += dt / 2.f;
	increased2 += dt / 2.f;

	if (decreased < 0.f)
	{
		increased1 += abs(decreased);
		decreased = 0.f;
		increased2 = 1.f - increased1;
	}

	if (increased1 > 1.f)
	{
		increased1 = 1.f;
		decreased = 0.f;
		increased2 = 0.f;
	}
	if (increased2 > 1.f)
	{
		increased2 = 1.f;
		decreased = 0.f;
		increased1 = 0.f;
	}

}

void cFormationGameManager::HandleInput(float dt)
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
		state = AI::AI_STATE::FLOCKING;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_7))
	{
		state = AI::AI_STATE::FORMATION;
		NotifyStateChange(state);
	}
	else if (Input::KeyUp(GLFW_KEY_8))
	{
		state = AI::AI_STATE::PATH_FOLLOW;
		nodeStep = 1;
		if (nodeIndex < 0) nodeIndex = 0;
		NotifyStateChange(AI::AI_STATE::FORMATION); // STAY IN FORMATION WHILE FOLLOWING PATH
	}
	else if (Input::KeyUp(GLFW_KEY_9))
	{
		state = AI::AI_STATE::PATH_FOLLOW;
		nodeStep = -1;
		if (nodeIndex >= nodes.size()) nodeIndex = nodes.size() - 1;
		NotifyStateChange(AI::AI_STATE::FORMATION); // STAY IN FORMATION WHILE FOLLOWING PATH
	}
	else if (Input::KeyUp(GLFW_KEY_0))
	{
		state = AI::AI_STATE::FORMATION;
		for (unsigned int i = 0; i < nodes.size(); ++i)
		{
			nodes[i]->texture[0].SetTexture("blue.png");
		}
		NotifyStateChange(state);
	}

	if (Input::GetKey(GLFW_KEY_U))
	{
		IncreaseWeight(weight.separation, weight.alignment, weight.cohesion, dt);
		DisplayWeights();
	}
	if (Input::GetKey(GLFW_KEY_J))
	{
		DecreaseWeight(weight.separation, weight.alignment, weight.cohesion, dt);
		DisplayWeights();
	}
	if (Input::GetKey(GLFW_KEY_I))
	{
		IncreaseWeight(weight.alignment, weight.separation, weight.cohesion, dt);
		DisplayWeights();
	}
	if (Input::GetKey(GLFW_KEY_K))
	{
		DecreaseWeight(weight.alignment, weight.separation, weight.cohesion, dt);
		DisplayWeights();
	}
	if (Input::GetKey(GLFW_KEY_O))
	{
		IncreaseWeight(weight.cohesion, weight.separation, weight.alignment, dt);
		DisplayWeights();
	}
	if (Input::GetKey(GLFW_KEY_L))
	{
		DecreaseWeight(weight.cohesion, weight.separation, weight.alignment, dt);
		DisplayWeights();
	}

	if (Input::KeyUp(GLFW_KEY_ENTER))
	{
		screenLocked = !screenLocked;
		if (screenLocked)
			Input::LockCursor();
		else
			Input::UnlockCursor();
	}
}

void cFormationGameManager::HandleState(float dt)
{
	if (state == AI::AI_STATE::NONE)
	{
		state = AI::AI_STATE::FORMATION;
		NotifyStateChange(state);
	}
	else if (state == AI::AI_STATE::FORMATION)
	{
		velocity = vec3(0.f);
		transform.SetEulerRotation(vec3(0.f));
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
	else if (state == AI::AI_STATE::PATH_FOLLOW)
	{
		for (unsigned int i = 0; i < nodes.size(); ++i)
		{
			nodes[i]->texture[0].SetTexture("blue.png");
		}
		if (nodeIndex >= 0 && nodeIndex < nodes.size())
		{
			vec3 target = nodes[nodeIndex]->transform.Position();
			if (glm::distance(transform.Position(), target) < 0.2f)
			{
				nodeIndex += nodeStep;
				HandleState(dt);
				return;
			}
			nodes[nodeIndex]->texture[0].SetTexture("red.png");
			cDebugRenderer::Instance().addLine(transform.Position(), target, vec3(1.f, 1.f, 0.f), dt);
			vec3 desired = glm::normalize(target - transform.Position()) * maxVelocity;
			vec3 steer = desired - velocity;
			velocity += steer * dt;

			if (velocity.length() > maxVelocity) steer = glm::normalize(velocity) * maxVelocity;
		}
		else
		{
			velocity = vec3(0.f);
		}

		if (glm::length(velocity) > 0.3f)
		{
			vec3 normalized = glm::normalize(vec3(velocity.x, 0.f, velocity.z));
			transform.Rotation(Mathf::RotationFromTo(vec3(0.f, 0.f, -1.f), normalized));
		}
		else
		{
			transform.SetEulerRotation(vec3(0.f));
		}

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
		velocity = vec3(0.f);
		transform.SetEulerRotation(vec3(0.f));
	}

	transform.Position(transform.Position() + velocity * dt);
}

void cFormationGameManager::DisplayWeights()
{
	std::cout << std::setprecision(2);
	std::cout << "Separation (" << weight.separation << ") ";
	std::cout << "Alignment (" << weight.alignment << ") ";
	std::cout << "Cohesion (" << weight.cohesion << ")" << std::endl;
}

void cFormationGameManager::NotifyStateChange(AI::AI_STATE _state)
{
	for (AI::cFormationBehaviour* agent : agents)
	{
		agent->state = _state;
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
		positions[i].x = x;
		positions[i].z = z;
		agents[i]->target = (positions[i] * transform.Rotation()) + pos;
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
		positions[i] = vec3(x, 0.f, 0.f);
		positions[i+1u] = vec3(x2, 0.f, 0.f);

		agents[i]->target = (positions[i] * transform.Rotation()) + pos;
		agents[i+1u]->target = (positions[i+1u] * transform.Rotation()) + pos;

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

	agents[0u]->target = (vec3(0.f, 0.f, 1.f) * dist) * transform.Rotation() + pos;
	agents[1u]->target = (vec3(-1.f, 0.f, 1.f) * dist)* transform.Rotation() + pos;
	agents[2u]->target = (vec3(1.f, 0.f, 1.f) * dist)* transform.Rotation() + pos;
	agents[3u]->target = (vec3(2.f, 0.f, 1.f) * dist)* transform.Rotation() + pos;

	agents[4u]->target = (vec3(2.f, 0.f, 0.f) * dist)* transform.Rotation() + pos;
	agents[5u]->target = (vec3(2.f, 0.f, -1.f) * dist)* transform.Rotation() + pos;
	agents[6u]->target = (vec3(2.f, 0.f, -2.f) * dist)* transform.Rotation() + pos;
	agents[7u]->target = (vec3(1.f, 0.f, -2.f) * dist)* transform.Rotation() + pos;

	agents[8u]->target = (vec3(0.f, 0.f, -2.f) * dist)* transform.Rotation() + pos;
	agents[9u]->target = (vec3(-1.f, 0.f, -2.f) * dist)* transform.Rotation() + pos;
	agents[10u]->target = (vec3(-1.f, 0.f, -1.f) * dist)* transform.Rotation() + pos;
	agents[11u]->target = (vec3(-1.f, 0.f, 0.f) * dist)* transform.Rotation() + pos;
}

void cFormationGameManager::SetLine()
{
	vec3 pos = transform.Position();

	float dist = 3.0f;

	char index = 0;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[index]->target = vec3((-6.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
}

void cFormationGameManager::SetRows()
{
	vec3 pos = transform.Position();

	float dist = 3.0f;
	char index = 0;

	agents[0u]->target = vec3((-3.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[1u]->target = vec3((-3.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[2u]->target = vec3((-3.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[3u]->target = vec3((-3.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[4u]->target = vec3((-3.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;
	agents[5u]->target = vec3((-3.f + index++) * dist, 0.f, 0.f) * transform.Rotation() + pos;

	index = 0;
	agents[6u]->target = vec3((-3.f + index++) * dist, 0.f, dist) * transform.Rotation() + pos;
	agents[7u]->target = vec3((-3.f + index++) * dist, 0.f, dist) * transform.Rotation() + pos;
	agents[8u]->target = vec3((-3.f + index++) * dist, 0.f, dist) * transform.Rotation() + pos;
	agents[9u]->target = vec3((-3.f + index++) * dist, 0.f, dist) * transform.Rotation() + pos;
	agents[10u]->target = vec3((-3.f + index++) * dist, 0.f, dist) * transform.Rotation() + pos;
	agents[11u]->target = vec3((-3.f + index++) * dist, 0.f, dist) * transform.Rotation() + pos;
}
