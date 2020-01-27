#pragma once

#include "cRigidBody.h"
#include "shapes.h"
#include <vector>
#include <glm/vec3.hpp>

namespace phys
{
	class cWorld
	{
	public:
		~cWorld();

		inline void GetGravity(glm::vec3& gravityOut) { gravityOut = mGravity; }
		inline void SetGravity(const glm::vec3& gravity) { mGravity = gravity; }

		void Update(float dt);

		bool AddRigidBody(cRigidBody* rigidBody);
		// what does the return bool mean?
		bool RemoveRigidBody(cRigidBody* rigidBody);
	protected:
		bool Collide(cRigidBody* bodyA, cRigidBody* bodyB);
		bool CollideSpherePlane(cRigidBody* sphereBody, const cSphere* sphereShape,
								cRigidBody* planeBody, const cPlane* planeShape);
		bool CollideSphereSphere(cRigidBody* sphereBodyA, const cSphere* sphereShapeA,
								 cRigidBody* sphereBodyB, const cSphere* sphereShapeB);

	private:
		glm::vec3 mGravity;
		std::vector<cRigidBody*> mBodies;
		float last_dt;
	};
}