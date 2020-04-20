#include "cFuzzyLogicBehaviour.h"
#include <Rendering/RenderingEngine.h>
#include <Physics/global_physics.h>

#define MAX_DISTANCE 2.f

namespace AI
{
	void cFuzzyLogicBehaviour::start()
	{
		mTurnSpeed = 5.f;
		mMovementSpeed = 5.f;
		directions = new vec3[5];
		distances = new float[5];
		rigidBody = parent.GetComponent<nPhysics::iPhysicsComponent>();
	}

	void cFuzzyLogicBehaviour::update(float dt)
	{
		mDt = dt;

		// Get the ray directions based on current rotation
		CalculateDirections();

		// Shoot 5 rays
		for (unsigned int i = 0; i < 5; ++i)
		{
			vec3& rd = directions[i];
			vec3 ro = transform.Position();
			distances[i] = MAX_DISTANCE;
			vec3 colour(1.f);

			// Raycast
			auto* result = g_PhysicsWorld->RayCast(ro, rd, MAX_DISTANCE, rigidBody);

			// If the ray hit
			if (result)
			{
				distances[i] = glm::distance(ro, result->hitPoint);
				colour = vec3(1.f, 0.f, 0.f);
				delete result;
			}

			// Render the rays
			RenderingEngine::Instance().DrawLine(ro, ro + rd * distances[i], colour);
		}

		// Turn the vehicle
		Turn();

		// Calculate the vehicle's speed
		CalculateSpeed();

		// Move the vehicle
		rigidBody->SetVelocity(speed * directions[2]);
	}

	void cFuzzyLogicBehaviour::CalculateDirections()
	{
		directions[2] = transform.Rotation() * vec3(0.f, 0.f, 1.f);
		directions[0] = transform.Rotation() * quat(vec3(0.f, glm::radians(60.f), 0.f)) * vec3(0.f, 0.f, 1.f);
		directions[1] = transform.Rotation() * quat(vec3(0.f, glm::radians(30.f), 0.f)) * vec3(0.f, 0.f, 1.f);
		directions[4] = transform.Rotation() * quat(vec3(0.f, glm::radians(-60.f), 0.f)) * vec3(0.f, 0.f, 1.f);
		directions[3] = transform.Rotation() * quat(vec3(0.f, glm::radians(-30.f), 0.f)) * vec3(0.f, 0.f, 1.f);
	}

	// Calculate Turn
	void cFuzzyLogicBehaviour::Turn()
	{
		float d = 0.f;
		d -= (1.f - (distances[1] / MAX_DISTANCE));
		d += (1.f - (distances[3] / MAX_DISTANCE));
		d -= (1.f - (distances[0] / MAX_DISTANCE));
		d += (1.f - (distances[4] / MAX_DISTANCE));
		d /= 2.f;

		quat turn(vec3(0.f, d * mDt * mTurnSpeed, 0.f));
		transform.rotation *= turn;
	}

	// Calculate Movement Speed
	void cFuzzyLogicBehaviour::CalculateSpeed()
	{
		float ratio = distances[2] / MAX_DISTANCE;
		ratio *= (distances[1] / MAX_DISTANCE);
		ratio *= (distances[3] / MAX_DISTANCE);
		ratio -= 0.5f;
		speed = ratio * mMovementSpeed;
	}

	// Destroy Object
	void cFuzzyLogicBehaviour::OnDestroy()
	{
		delete directions;
		directions = 0;
		delete distances;
		distances = 0;
	}

}