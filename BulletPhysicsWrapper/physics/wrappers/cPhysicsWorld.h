#pragma once


#include <interfaces/physics/iPhysicsWorld.h>
#include "../bullet/btBulletDynamicsCommon.h"

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
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
		btDefaultCollisionConfiguration* mCollisionConfiguration;
		btCollisionDispatcher* mDispatcher;
		btBroadphaseInterface* mOverlappingPairCache;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld* mDynamicsWorld;
		//phys::cWorld* mWorld;

		std::vector<iPhysicsComponent*> components;
	};
}