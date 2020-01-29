#include "cWorld.h"
#include <algorithm>

#define FIND(container, item) std::find(container.begin(), container.end(), item)

phys::cWorld::~cWorld()
{
	last_dt = 0.f;
	mGravity = glm::vec3(0.f, -9.8f, 0.f);
}

void phys::cWorld::Update(float dt)
{
	last_dt = dt;
	size_t numBodies = mBodies.size();
	if (numBodies == 0) return;
	// Step 1: Integrate
	for (size_t c = 0; c < numBodies; c++)
	{
		Integrate(mBodies[c]);
	}

	// Step 2: Handle Collisions
	for (size_t idxA = 0; idxA < numBodies - 1; ++idxA)
	{
		for (size_t idxB = idxA + 1; idxB < numBodies; idxB++)
		{
			Collide(mBodies[idxA], mBodies[idxB]);
		}
	}
}

bool phys::cWorld::AddRigidBody(cRigidBody* rigidBody)
{
	if (!rigidBody) return false;
	auto itbody = FIND(mBodies, rigidBody);
	if (itbody == mBodies.end())
	{
		mBodies.push_back(rigidBody);
		return true;
	}
	return false;
}

bool phys::cWorld::RemoveRigidBody(cRigidBody* rigidBody)
{
	if (!rigidBody) return false;
	auto itbody = FIND(mBodies, rigidBody);
	if (itbody == mBodies.end()) return false; // wasn't removed
	mBodies.erase(itbody);
	return true;
}

bool phys::cWorld::Collide(cRigidBody* bodyA, cRigidBody* bodyB)
{
	eShapeType shapeA = bodyA->GetShape();
	eShapeType shapeB = bodyB->GetShape();

	if (shapeA == eShapeType::plane)
	{
		if (shapeB == eShapeType::plane)
		{
			return false;
		}
		if (shapeB == eShapeType::sphere)
		{
			return CollideSpherePlane(bodyB, dynamic_cast<const cSphereBody*>(bodyB),
									  bodyA, dynamic_cast<const cPlaneBody*>(bodyA));
		}
	}
	if (shapeA == eShapeType::sphere)
	{
		if (shapeB == eShapeType::sphere)
		{
			// sphere-sphere collision
			return CollideSphereSphere(bodyA, dynamic_cast<const cSphereBody*>(bodyA),
									   bodyB, dynamic_cast<const cSphereBody*>(bodyB));
		}
		if (shapeB == eShapeType::plane)
		{
			return CollideSpherePlane(bodyA, dynamic_cast<const cSphereBody*>(bodyA),
									  bodyB, dynamic_cast<const cPlaneBody*>(bodyB));
		}
	}

	return false;
}

bool phys::cWorld::CollideSpherePlane(cRigidBody* sphereBody, const cSphereBody* sphereShape,
									  cRigidBody* planeBody, const cPlaneBody* planeShape)
{
	// TODO: take a look at chapter 5.5
	//		make your own version of IntersectMovingSpherePlane

	float collisionValue = 0.3f;// IntersectMovingSpherePlane(...); // TODO

	return false;
}

bool phys::cWorld::CollideSphereSphere(cRigidBody* sphereBodyA, const cSphereBody* sphereShapeA, cRigidBody* sphereBodyB, const cSphereBody* sphereShapeB)
{
	return false;
}

void phys::cWorld::Integrate(cRigidBody* body)
{
	body->mVelocity += mGravity * last_dt;
	body->mVelocity += body->mAcceleration * last_dt;
	body->mPosition += body->mVelocity * last_dt;
}
