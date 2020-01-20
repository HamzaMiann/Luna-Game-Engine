#include "cRigidBody.h"
#include <Physics/PhysicsEngine.h>

cRigidBody::cRigidBody(iObject* obj)
	: transform(obj->transform)
{
	rigidBody = this;
	velocity = vec3(0.f);
	acceleration = vec3(0.f);
	force = vec3(0.f);
	previousPos = transform.pos;
	gravityScale = 0.f;
	inverseMass = 1.f;
	PhysicsEngine::Instance()->Register(this);
}

cRigidBody::~cRigidBody()
{
	PhysicsEngine::Instance()->Unregister(this);
}
