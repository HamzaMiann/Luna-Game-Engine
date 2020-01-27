#pragma once


#include <interfaces/physics/iPhysicsWorld.h>

namespace phys
{
	class cWorld;
}

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();
		virtual void Update(float dt);
		virtual bool AddComponent(iPhysicsComponent* component) override;
		virtual bool RemoveComponent(iPhysicsComponent* component) override;

	private:
		phys::cWorld* mWorld;
	};
}