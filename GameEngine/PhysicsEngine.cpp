
#include "PhysicsEngine.h"
#include "AudioEngine.hpp"
#include <math.h>
using namespace std;

//AudioEngine* audengine = &AudioEngine::Instance();

void PhysicsUpdate(std::vector<cGameObject> & vecGameObjects, float deltaTime)
{
	glm::vec3 gravity = glm::vec3(0.f, -1.0f, 0.f);
	float friction = .8f;
	float drag = .9f;

	for (size_t i = 0; i < vecGameObjects.size(); ++i)
	{
		// Forward Explicit Euler Integration
		cGameObject* pObj = &(vecGameObjects[i]);

		if (pObj->inverseMass == 0.f) continue;

		pObj->velocity += (pObj->acceleration * deltaTime);
		pObj->velocity += (gravity * deltaTime);
		if ((pObj->positionXYZ.y + (pObj->velocity * deltaTime)).y <= 0.5f)
		{
			bool shouldPlaySound = true;
			float lastY = pObj->positionXYZ.y;
			float newY = (pObj->positionXYZ.y + (pObj->velocity * deltaTime)).y;
			if (abs(lastY - newY) <= 0.5f * deltaTime) pObj->velocity.y *= friction;
			if (abs(lastY - newY) <= 0.4f * deltaTime) pObj->velocity.y *= friction;
			if (abs(lastY - newY) <= 0.2f * deltaTime)
			{
				pObj->velocity.y = 0.f;
				shouldPlaySound = false;
			}
			pObj->velocity.y = pObj->velocity.y * -1.f * friction;
			pObj->velocity.x = pObj->velocity.x * drag;
			pObj->velocity.z = pObj->velocity.z * drag;
			/*if (shouldPlaySound)
			{
				audengine->PlaySound("ball");
			}*/

		}
		pObj->positionXYZ += (pObj->velocity * deltaTime);

		

		// Test to see if it's hit the cube

		// Determine the object new position

		// Position ( x = 0 )

		// Velocity ( Vx = 1.0f / second )

		// DeltaX = Vx * deltaTime
		//		= 1.0f / sec * 1.0 second

		// Position += DeltaX
		// Position += Vx * deltaTime

		// Velocity += DeltaV * detlaTime
		// Velocity += Ax * deltaTime

	}

	return;
}