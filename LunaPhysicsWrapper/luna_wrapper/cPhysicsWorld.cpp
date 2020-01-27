#include "cPhysicsWorld.h"

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

void nPhysics::cPhysicsWorld::AddObject()
{
}

void nPhysics::cPhysicsWorld::RemoveObject()
{
}
