#pragma once


#include <interfaces/physics/iPhysicsWorld.h>
#include "../bullet/btBulletDynamicsCommon.h"
#include "../bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "../bullet/BulletCollision/Gimpact/btGImpactShape.h"
#include "cPhysicsDebugDrawer.h"
#include "../bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cPhysicsWorld : public iPhysicsWorld
	{
	public:
		cPhysicsWorld();
		virtual ~cPhysicsWorld();

		virtual void Update(float dt);

		virtual bool AddComponent(iPhysicsComponent* component) override;
		virtual bool AddCharacter(iPhysicsComponent* component);
		virtual bool AddObject(iPhysicsComponent* component);
		virtual bool RemoveComponent(iPhysicsComponent* component) override;
		virtual bool RemoveCharacter(iPhysicsComponent* component);
		virtual bool RemoveObject(iPhysicsComponent* component);

		virtual std::vector<RayCastResult> RayCastAll(vec3 ro, vec3 rd, float t) override;
		virtual RayCastResult* RayCast(vec3 ro, vec3 rd, float t) override;

		virtual void SetDebugRenderer(iPhysicsDebugRenderer* renderer) override;

		void AddConstraint(btTypedConstraint* constraint);

	private:
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
		btDefaultCollisionConfiguration* mCollisionConfiguration;
		btGhostPairCallback* mGhostPairCallback;
		btCollisionDispatcher* mDispatcher;
		btBroadphaseInterface* mOverlappingPairCache;
		btSequentialImpulseConstraintSolver* mSolver;
		btDiscreteDynamicsWorld* mDynamicsWorld;

		std::vector<iPhysicsComponent*> components;

		cPhysicsDebugDrawer* debugDrawer;
	};
}