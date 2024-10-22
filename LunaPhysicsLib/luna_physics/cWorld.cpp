#include "cWorld.h"
#include <algorithm>
#include "nCollide.h"
#include <glm/gtx/projection.hpp>

#define FIND(container, item) std::find(container.begin(), container.end(), item)

namespace phys
{
	cWorld::~cWorld()
	{
		mDt = 0.f;
		T = 0.f;
	}

	void cWorld::Update(float dt)
	{
		mDt = dt;
		size_t numBodies = mBodies.size();

		if (numBodies == 0) return;

		// Step 1: Integrate
		for (size_t c = 0; c < numBodies; c++)
		{
			// integrate rigid body
			if (mBodies[c]->GetBodyType() == eBodyType::rigid)
				Integrate(reinterpret_cast<cRigidBody*>(mBodies[c]), dt);

			// self-integrate soft body
			else if (mBodies[c]->GetBodyType() == eBodyType::soft)
				reinterpret_cast<cSoftBody*>(mBodies[c])->Integrate(dt, mGravity);
		}

		// Step 2: Handle Collisions
		for (size_t idxA = 0; idxA < numBodies - 1; ++idxA)
		{
			for (size_t idxB = idxA + 1; idxB < numBodies; idxB++)
			{
				iBody* A = mBodies[idxA];
				iBody* B = mBodies[idxB];

				// Collide bodies
				Collide(A, B);
			}
		}
		// Step 3: Clear the accelerations!
		for (size_t i = 0; i < numBodies; i++)
		{
			if (mBodies[i]->GetBodyType() == eBodyType::rigid)
				reinterpret_cast<cRigidBody*>(mBodies[i])->mAcceleration = glm::vec3(0.f, 0.f, 0.f);
		}

		// Step 4: Tell everyone about all the collisions!

		// set the previous time (for RK4 integration next frame)
		T += dt;
	}

	bool cWorld::AddBody(iBody* Body)
	{
		if (!Body) return false;
		auto itbody = FIND(mBodies, Body);
		if (itbody == mBodies.end())
		{
			mBodies.push_back(Body);
			return true;
		}
		return false;
	}

	bool cWorld::RemoveBody(iBody* Body)
	{
		if (!Body) return false;
		auto itbody = FIND(mBodies, Body);
		if (itbody == mBodies.end()) return false; // wasn't removed
		mBodies.erase(itbody);
		return true;
	}


	void cWorld::Integrate(cRigidBody* body, float dt)
	{
		if (body->IsStatic()) return;

		body->mPreviousPosition = body->mPosition;

		// RK4 integration
		mIntegrator.RK4(body->mPosition, body->mVelocity, body->mAcceleration, mGravity, body->mGravityFactor, dt, T);
	}

	void cWorld::Collide(iBody* A, iBody* B)
	{
		// SOFT BODY COLLISIONS
		if (A->GetBodyType() == eBodyType::soft &&
			B->GetBodyType() == eBodyType::rigid)
		{
			reinterpret_cast<cSoftBody*>(A)->CollideWith(reinterpret_cast<cRigidBody*>(B));
		}
		else if (B->GetBodyType() == eBodyType::soft &&
			A->GetBodyType() == eBodyType::rigid)
		{
			reinterpret_cast<cSoftBody*>(B)->CollideWith(reinterpret_cast<cRigidBody*>(A));
		}

		// RIGID BODY COLLISIONS
		else
		{
			cRigidBody* rA = reinterpret_cast<cRigidBody*>(A);
			cRigidBody* rB = reinterpret_cast<cRigidBody*>(B);
			if (Collide(rA, rB))
			{
				rA->Collided(rB);
				rB->Collided(rA);
			}
		}
	}

	bool cWorld::Collide(cRigidBody* bodyA, cRigidBody* bodyB)
	{
		eShapeType shapeA = bodyA->GetShapeType();
		eShapeType shapeB = bodyB->GetShapeType();

		if (shapeA == eShapeType::plane)
		{
			if (shapeB == eShapeType::plane)
			{
				return false;
			}
			if (shapeB == eShapeType::sphere)
			{
				return CollideSpherePlane(bodyB, reinterpret_cast<cSphere*>(bodyB->GetShape()),
										  bodyA, reinterpret_cast<cPlane*>(bodyA->GetShape()));
			}
		}
		if (shapeA == eShapeType::sphere)
		{
			if (shapeB == eShapeType::sphere)
			{
				// sphere-sphere collision
				return CollideSphereSphere(bodyA, reinterpret_cast<cSphere*>(bodyA->GetShape()),
										   bodyB, reinterpret_cast<cSphere*>(bodyB->GetShape()));
			}
			if (shapeB == eShapeType::plane)
			{
				return CollideSpherePlane(bodyA, reinterpret_cast<cSphere*>(bodyA->GetShape()),
										  bodyB, reinterpret_cast<cPlane*>(bodyB->GetShape()));
			}
		}

		return false;
	}


	bool cWorld::CollideSpherePlane(cRigidBody* sphereBody, cSphere* sphereShape,
										  cRigidBody* planeBody, cPlane* planeShape)
	{
		// TODO: take a look at chapter 5.5
		//		make your own version of IntersectMovingSpherePlane

		// page 127 closest point on plane
		// intersect moving sphere

		glm::vec3 c = sphereBody->mPreviousPosition;
		float r = sphereShape->GetRadius();
		glm::vec3 v = sphereBody->mPosition - sphereBody->mPreviousPosition;
		glm::vec3 n = planeShape->GetNormal();
		float d = planeShape->GetConstant();
		float t(0.f);
		glm::vec3 q;
		int result = nCollide::intersect_moving_sphere_plane(c, r, v, n, d, t, q);

		if (result == 0)
		{
			// no collision!
			return false;
		}
		if (result == -1)
		{
			// already colliding at the beginning of the timestep

			glm::vec3 pointOnPlane = nCollide::closest_point_on_plane(sphereBody->mPreviousPosition,
																	  planeShape->GetNormal(),
																	  planeShape->GetConstant());

			// figure out an impulse that should have the sphere escape the plane
			float distance = glm::distance(sphereBody->mPreviousPosition,
										   pointOnPlane);

			float targetDistance = r;
			glm::vec3 impulse = n * (targetDistance - distance) / mDt;

			// reset
			sphereBody->mPosition = sphereBody->mPreviousPosition;

			// apply impulse
			sphereBody->mVelocity += impulse;

			// re-integrate
			Integrate(sphereBody, mDt);

			return true;
		}
		// collided!

		// REFLECT!
		sphereBody->mVelocity = glm::reflect(sphereBody->mVelocity, planeShape->GetNormal());

		// Lose some energy
		glm::vec3 nComponent = glm::proj(sphereBody->mVelocity, planeShape->GetNormal());
		sphereBody->mVelocity -= nComponent * 0.2f;

		// rewind
		sphereBody->mPosition = (c + v * t);

		// integrate
		Integrate(sphereBody, mDt * (1.f - t));

		return true;
	}

	bool cWorld::CollideSphereSphere(cRigidBody* bodyA, cSphere* sphereShapeA,
										   cRigidBody* bodyB, cSphere* sphereShapeB)
	{
		glm::vec3 offsetA = sphereShapeA->GetOffset();
		glm::vec3 offsetB = sphereShapeB->GetOffset();
		glm::vec3 cA = bodyA->mPreviousPosition + offsetA;
		glm::vec3 cB = bodyB->mPreviousPosition + offsetB;
		glm::vec3 vA = (bodyA->mPosition + offsetA) - (bodyA->mPreviousPosition + offsetA);
		glm::vec3 vB = (bodyB->mPosition + offsetB) - (bodyB->mPreviousPosition + offsetB);
		float rA = sphereShapeA->GetRadius();
		float rB = sphereShapeB->GetRadius();
		float t(0.f);
		int result = nCollide::intersect_moving_sphere_sphere(cA, rA, vA, cB, rB, vB, t);

		if (result == 0)
		{
			// not colliding!
			return false;
		}
		// get the masses
		float mb = bodyB->mMass;
		float ma = bodyA->mMass;
		float mt = ma + mb;
		if (result == -1)
		{
			// already colliding at the beginning of the timestep
			// use an impulse to push them apart

			float initialDistance = glm::distance(bodyA->mPreviousPosition + offsetA,
												  bodyB->mPreviousPosition + offsetB);

			float targetDistance = rA + rB;

			glm::vec3 impulseToA = glm::normalize((bodyA->mPreviousPosition + offsetA)
												  - (bodyB->mPreviousPosition + offsetB));

			impulseToA *= targetDistance - initialDistance;

			// back to ones!
			bodyA->mPosition = bodyA->mPreviousPosition;
			bodyB->mPosition = bodyB->mPreviousPosition;

			// apply the impulse
			bodyA->mVelocity += impulseToA * (mb / mt);
			bodyB->mVelocity -= impulseToA * (ma / mt);

			Integrate(bodyA, mDt);
			Integrate(bodyB, mDt);

			return true;
		}
		// collided!

		// back to ones!
		bodyA->mPosition = bodyA->mPreviousPosition + vA * t;
		bodyB->mPosition = bodyB->mPreviousPosition + vB * t;

		vA = bodyA->mVelocity;
		vB = bodyB->mVelocity;

		// reflect
		float c = 0.1f;
		bodyA->mVelocity = (c * mb * (vB - vA) + ma * vA + mb * vB) / mt;
		bodyB->mVelocity = (c * ma * (vA - vB) + ma * vA + mb * vB) / mt;

		// integrate
		Integrate(bodyA, mDt * (1.f - t));
		Integrate(bodyB, mDt * (1.f - t));


		return true;
	}

}