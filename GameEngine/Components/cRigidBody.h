#pragma once

#include <iObject.h>

class cRigidBody : public iComponent
{
private:
	typedef glm::vec3 vec3;

protected:
	friend class PhysicsEngine;

	vec3 velocity;
	vec3 acceleration;
	vec3 force;
	vec3 previousPos;

public:

	sTransform&	transform;
	float		gravityScale;
	float		inverseMass;

	cRigidBody(iObject* obj)
		: transform(obj->transform)
	{
		rigidBody		= this;
		velocity		= vec3(0.f);
		acceleration	= vec3(0.f);
		force			= vec3(0.f);
		previousPos		= transform.pos;
		gravityScale	= 0.f;
		inverseMass		= 1.f;
	}

	virtual ~cRigidBody() {}

	void	AddForce(vec3 newtons) { force += newtons; }
	void	SetForce(vec3 newtons) { force = newtons; }
	vec3	GetForce() { return force; }

	vec3	GetVelocity() { return velocity; }
	void	SetVelocity(vec3 vel) { velocity = vel; }

	vec3	GetAcceleration() { return acceleration; }
	void	SetAcceleration(vec3 accel) { acceleration = accel; }

};