#pragma once


#include <interfaces/physics/iPhysicsWorld.h>
#include <luna_physics/cWorld.h>

#pragma comment (lib, "LunaPhysicsLib.lib")

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();

		virtual void Update(float dt);

		virtual bool AddComponent(iPhysicsComponent* component);
		virtual bool RemoveComponent(iPhysicsComponent* component);

	private:
		phys::cWorld* mWorld;
		std::vector<iPhysicsComponent*> components;
	};
}