#include "cPhysicsWorld.h"
#include <luna_physics/cWorld.h>

nPhysics::cPhysicsWorld::cPhysicsWorld()
{
	mWorld = new phys::cWorld;
}

nPhysics::cPhysicsWorld::~cPhysicsWorld()
{
	delete mWorld;
}

void nPhysics::cPhysicsWorld::Update(float dt)
{
	mWorld->Update(dt);
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component)
{
	return mWorld->AddRigidBody(dynamic_cast<phys::cRigidBody*>(component));
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
{
	return mWorld->RemoveRigidBody(dynamic_cast<phys::cRigidBody*>(component));
}
