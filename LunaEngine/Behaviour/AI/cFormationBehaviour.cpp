#include "cFormationBehaviour.h"
#include <Physics/Mathf.h>

void AI::cFormationBehaviour::start()
{
	maxVelocity = 10.f;
	rigidBody = parent.GetComponent<nPhysics::iPhysicsComponent>();
	animator = parent.GetComponent<cAnimationController>();
}

void AI::cFormationBehaviour::update(float dt)
{
	if (state == AI_STATE::FORMATION)
	{
		Seek(dt);
	}
	else if (state == AI_STATE::FLOCKING)
	{
		// TODO
	}

	float length = 0.f;

	if (rigidBody)
	{
		vec3 velocity = rigidBody->GetVelocity();
		velocity.y = 0.f;
		float length = glm::length(velocity);
		if (length > 0.1)
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

    vec3 desiredVelocity = glm::normalize(target - transform.Position());
    desiredVelocity *= maxVelocity;

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
