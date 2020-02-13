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

		inline virtual std::vector<iPhysicsComponent*> RayCastAll(vec3 ro, vec3 rd, float t) override
		{
			return std::vector<iPhysicsComponent*>();
		}

		inline virtual iPhysicsComponent* RayCast(vec3 ro, vec3 rd, float t) override
		{
			return nullptr;
		}

	private:
		phys::cWorld* mWorld;
		std::vector<iPhysicsComponent*> components;
	};
}