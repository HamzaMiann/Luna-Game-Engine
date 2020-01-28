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
		bool CollideSpherePlane(cRigidBody* sphereBody, const cSphereBody* sphereShape,
								cRigidBody* planeBody, const cPlaneBody* planeShape);
		bool CollideSphereSphere(cRigidBody* sphereBodyA, const cSphereBody* sphereShapeA,
								 cRigidBody* sphereBodyB, const cSphereBody* sphereShapeB);
		
		void Integrate(cRigidBody* body);

	private:
		glm::vec3 mGravity;
		std::vector<cRigidBody*> mBodies;
		float last_dt;
	};
}