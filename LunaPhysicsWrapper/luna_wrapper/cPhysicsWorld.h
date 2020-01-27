#pragma once


#include <iPhysicsWorld.h>
#include <cWorld.h>

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();
		virtual void Update(float dt);
		virtual void AddObject(/* todo physics object */);
		virtual void RemoveObject(/* todo physics object */);

	private:
		phys::cWorld* mWorld;
	};
}