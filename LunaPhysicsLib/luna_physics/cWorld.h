#pragma once

#include "shapes.h"
#include "cIntegrator.h"
#include "iBody.h"
#include "cSoftBody.h"
#include "cRigidBody.h"
#include <vector>

namespace phys
{
	class cWorld
	{
	public:
		~cWorld();

		inline void GetGravity(glm::vec3& gravityOut) { gravityOut = mGravity; }
		inline void SetGravity(const glm::vec3& gravity) { mGravity = gravity; }

		void Update(float dt);

		bool AddBody(iBody* rigidBody);
		bool RemoveBody(iBody* rigidBody);

	protected:

		void Integrate(cRigidBody* body, float dt);

		void Collide(iBody* A, iBody* B);
		bool Collide(cRigidBody* bodyA, cRigidBody* bodyB);
		bool CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape,
								cRigidBody* planeBody,  cPlane* planeShape);
		bool CollideSphereSphere(cRigidBody* bodyA, cSphere* sphereShapeA,
								 cRigidBody* bodyB, cSphere* sphereShapeB);


	private:
		glm::vec3 mGravity = glm::vec3(0.f, -9.8f, 0.f);
		std::vector<iBody*> mBodies;
		float mDt;
		float T;
		cIntegrator mIntegrator;
	};
}