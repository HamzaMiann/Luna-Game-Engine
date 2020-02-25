#include "cFormationBehaviour.h"
#include <Physics/Mathf.h>
#include <Behaviour/Managers/cFormationGameManager.h>
#include <DebugRenderer/cDebugRenderer.h>

void AI::cFormationBehaviour::start()
{
	maxVelocity = 10.f;
	slowingRadius = 5.f;
	rigidBody = parent.GetComponent<nPhysics::iPhysicsComponent>();
	animator = parent.GetComponent<cAnimationController>();
}

void AI::cFormationBehaviour::update(float dt)
{
	if (state == AI_STATE::FORMATION)
	{
		maxVelocity = 20.f;
		Seek(dt);
	}
	else if (state == AI_STATE::FLOCKING)
	{
		maxVelocity = 10.f;
		if (rigidBody)
		{
			vec3 velocity = Flock(dt);
			rigidBody->AddVelocity(velocity);

			if (glm::length(rigidBody->GetVelocity()) > maxVelocity)
			{
				rigidBody->SetVelocity(glm::normalize(rigidBody->GetVelocity()) * maxVelocity);
			}
		}
	}

	if (rigidBody)
	{
		vec3 velocity = rigidBody->GetVelocity();
		velocity.y = 0.f;
		float length = glm::length(velocity);
		if (length > 0.3)
		{
			vec3 direction = glm::normalize(velocity);
			transform.Rotation(
				glm::slerp(
					transform.Rotation(),
					Mathf::RotationFromTo(
						vec3(0.f, 0.f, 1.f),
						direction
					),
					dt * 10.f
				)
			);
		}

		Animate(length);
	}
}

void AI::cFormationBehaviour::Seek(float dt)
{
    if (!rigidBody) return;

    vec3 desiredVelocity = target - transform.Position();
	float dist = glm::length(desiredVelocity);
	desiredVelocity = glm::normalize(desiredVelocity);

	if (dist < slowingRadius)
	{
		/* game object is approaching the target and slows down*/
		desiredVelocity = desiredVelocity * maxVelocity * (dist / slowingRadius);
	}
	else
	{
		/* target is far away from game object*/
		desiredVelocity *= maxVelocity;
	}

    vec3 steer = desiredVelocity - rigidBody->GetVelocity();


    /* add steering force to current velocity*/
    rigidBody->AddVelocity(steer * dt);

    if (glm::length(rigidBody->GetVelocity()) > maxVelocity)
    {
        rigidBody->SetVelocity(glm::normalize(rigidBody->GetVelocity()) * maxVelocity);
    }
}

void AI::cFormationBehaviour::Animate(float length)
{
	if (animator)
	{
		if (length > 5.f)
		{
			animator->SetAnimation("run");
		}
		else if (length > 0.7f)
		{
			animator->SetAnimation("walk");
		}
		else
		{
			animator->SetAnimation("idle");
		}
	}
}

vec3 AI::cFormationBehaviour::Flock(float dt)
{
	return
		Separation(dt) * manager->weight.separation
		+ Alignment(dt) * manager->weight.alignment
		+ Cohesion(dt) * manager->weight.cohesion;
}

vec3 AI::cFormationBehaviour::Separation(float dt)
{
	std::vector<cFormationBehaviour*>& boids = manager->agents;
	vec3 flee(0.f);
	int neighbourCount = 0;
	for (unsigned int i = 0; i < boids.size(); i++)
	{
		if (boids[i] == this) continue;

		float dist = glm::distance(transform.Position(), boids[i]->transform.Position());
		if ((dist > 0) && (dist < manager->separationRadius))
		{
			vec3 fleeVelocity = transform.Position() - boids[i]->transform.Position();
			fleeVelocity = glm::normalize(fleeVelocity);
			fleeVelocity /= dist * 0.02f; //scale based on distancetotal
			flee += fleeVelocity;
			neighbourCount++;
		}
	}

	vec3 steerForce(0.f);
	vec3 desiredVelocity(0.f);
	float maxForce = 1.f;
	if (neighbourCount > 0)
	{
		desiredVelocity = flee / (float)neighbourCount;

		//desiredVelocity = glm::normalize(desiredVelocity);
		//desiredVelocity *= maxVelocity;
		cDebugRenderer::Instance().addLine(transform.Position(), desiredVelocity * maxForce + transform.Position(), vec3(0.f, 1.f, 0.f), dt);

		steerForce = (desiredVelocity - rigidBody->GetVelocity());
		steerForce *= maxForce;
		if (glm::length(steerForce) > maxVelocity)
		{
			steerForce = glm::normalize(steerForce) * maxVelocity;
		}
		steerForce = desiredVelocity * maxForce;
	}
	return steerForce * dt;
}

vec3 AI::cFormationBehaviour::Alignment(float dt)
{
	vec3 totalVelocity(0.f);
	int neighbourCount = 0;
	std::vector<cFormationBehaviour*>& boids = manager->agents;

	for (unsigned int i = 0; i < boids.size(); i++)
	{
		if (boids[i] == this) continue;

		float dist = glm::distance(transform.Position(), boids[i]->transform.Position());
		if ((dist > 0) && (dist < manager->alignmentRadius))
		{
			totalVelocity += boids[i]->rigidBody->GetVelocity();
			neighbourCount++;
		}
	}

	vec3 steerForce(0.f);
	vec3 desiredVelocity(0.f);
	float maxForce = 1.f;
	if (neighbourCount > 0)
	{
		desiredVelocity = totalVelocity / (float)neighbourCount;
		desiredVelocity = glm::normalize(desiredVelocity);

		desiredVelocity *= maxVelocity;

		//cDebugRenderer::Instance().addLine(transform.Position(), desiredVelocity + transform.Position(), vec3(0.f, 1.f, 1.f), dt);

		steerForce = (desiredVelocity - rigidBody->GetVelocity());
		steerForce *= maxForce;
	}
	return steerForce * dt;
}

vec3 AI::cFormationBehaviour::Cohesion(float dt)
{
	vec3 totalPosition(0.f);
	int neighbourCount = 0;
	std::vector<cFormationBehaviour*>& boids = manager->agents;
	for (unsigned int i = 0; i < boids.size(); i++)
	{
		if (boids[i] == this) continue;

		float dist = glm::distance(transform.Position(), boids[i]->transform.Position());
		if (dist < manager->cohesionRadius)
		{
			totalPosition += boids[i]->transform.Position();
			neighbourCount++;
		}
	}

	vec3 steerForce(0.f);
	if (neighbourCount > 0)
	{
		vec3 target = totalPosition / (float)neighbourCount;
		//target.y = transform.Position().y;


		vec3 desiredVelocity = glm::normalize(target - transform.Position());
		desiredVelocity *= maxVelocity;

		cDebugRenderer::Instance().addLine(transform.Position(), transform.Position() + desiredVelocity, vec3(1.f, 0.f, 0.f), dt);

		//vec3 desiredVelocity = glm::normalize(target - transform.Position());
		//vec3 steer = desiredVelocity - rigidBody->GetVelocity();

		//vec3 velocity = rigidBody->GetVelocity();
		///* add steering force to current velocity*/
		//velocity += steer * dt;

		//if (glm::length(velocity) > maxVelocity)
		//{
		//	velocity = glm::normalize(velocity) * maxVelocity;
		//}

		steerForce = (desiredVelocity - rigidBody->GetVelocity());
		steerForce = desiredVelocity;
	}
	return steerForce * dt;
}
