#include "cPhysicsWorld.h"
#include <luna_physics/cRigidBody.h>

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
	for (auto& component : components)
	{
		component->UpdateTransform();
	}
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component)
{
	components.push_back(component);
	return mWorld->AddRigidBody(dynamic_cast<phys::cRigidBody*>(component));
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
{
	auto i = std::find(components.begin(), components.end(), component);
	if (i != components.end())
	{
		components.erase(i);
	}
	return mWorld->AddRigidBody(dynamic_cast<phys::cRigidBody*>(component));
}