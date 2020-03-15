#include "cRigidBody.h"

#include <glm/gtc/matrix_transform.hpp>

phys::cRigidBody::cRigidBody(const sRigidBodyDef& def, iShape* shape)
	: mShape(shape),
	iBody(eBodyType::rigid),
	mPosition(def.Position),
	mPreviousPosition(def.Position),
	mVelocity(def.Velocity),
	mMass(def.Mass),
	mGravityFactor(def.GravityFactor),
	mAcceleration(glm::vec3(0.f))
{
	if (mMass == 0.f)
	{
		mInvMass = 0.f;
	}
	else
	{
		mInvMass = 1.f / mMass;
	}
}

void phys::cRigidBody::GetTransformFromBody(glm::mat4& transform)
{
	transform = glm::mat4(1.f);
	// rotations
	transform *= glm::translate(transform, mPosition);
}

void phys::cRigidBody::AddForceToCenter(const glm::vec3& force)
{
	mAcceleration += force * mInvMass;
}

void phys::cRigidBody::AddForceAtPoint(const glm::vec3& force, const glm::vec3& relativePoint)
{
	mAcceleration += force * mInvMass;
}
