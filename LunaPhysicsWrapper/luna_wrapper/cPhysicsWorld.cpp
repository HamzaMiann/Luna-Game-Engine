#include "cPhysicsWorld.h"
#include <luna_physics/iBody.h>

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
	return mWorld->AddBody(dynamic_cast<iBody*>(component));
}

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
{
	auto i = std::find(components.begin(), components.end(), component);
	if (i != components.end())
	{
		components.erase(i);
	}
	return mWorld->RemoveBody(dynamic_cast<iBody*>(component));
}