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

	cRigidBody(iObject* obj);

	virtual ~cRigidBody();

	void	AddForce(vec3 newtons) { force += newtons; }
	void	SetForce(vec3 newtons) { force = newtons; }
	vec3	GetForce() { return force; }

	vec3	GetVelocity() { return velocity; }
	void	SetVelocity(vec3 vel) { velocity = vel; }

	vec3	GetAcceleration() { return acceleration; }
	void	SetAcceleration(vec3 accel) { acceleration = accel; }

	virtual bool serialize(rapidxml::xml_node<>* root_node) override;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

};