#include "../nConvert.h"
#include "cBulletWrapperComponent.h"
#include "cCharacterComponent.h"
#include "cPhysicsWorld.h"
#include "cTriggerRegionComponent.h"

namespace nPhysics {


	cPhysicsWorld::cPhysicsWorld()
	{
		mCollisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		mOverlappingPairCache = new btDbvtBroadphase();
		mGhostPairCallback = new btGhostPairCallback();
		mOverlappingPairCache->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback);

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		mSolver = new btSequentialImpulseConstraintSolver;

		mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);

		mDynamicsWorld->setGravity(btVector3(0, -10, 0));

		debugDrawer = 0;

	}

	cPhysicsWorld::~cPhysicsWorld()
	{
		// remove all components first
		auto it = components.begin();
		while (components.size() > 0) components.erase(it);

		delete mDynamicsWorld;
		delete mDispatcher;
		delete mGhostPairCallback;
		delete mCollisionConfiguration;
		delete mOverlappingPairCache;
		delete mSolver;
	}

	void cPhysicsWorld::Update(float dt)
	{
		// Run the physics simulation
		mDynamicsWorld->stepSimulation(dt, 10);
		//if (mCollisionListener)
		//{
		//	// TODO: COLLISION LISTENING STUFF
		//}

		// Dispatch collision events
		for (unsigned int i = 0; i < mDispatcher->getNumManifolds(); ++i)
		{
			btPersistentManifold* manifold = mDispatcher->getManifoldByIndexInternal(i);
			iPhysicsComponent* bodyA = (iPhysicsComponent*)manifold->getBody0()->getUserPointer();
			iPhysicsComponent* bodyB = (iPhysicsComponent*)manifold->getBody1()->getUserPointer();
			bodyA->CollidedWith(bodyB);
			bodyB->CollidedWith(bodyA);
		}

		// Transform update for all parents
		for (iPhysicsComponent*& i : components)
		{
			auto wrapper = dynamic_cast<cBulletWrapperComponent*>(i);
			i->UpdateTransform();
		}

		// Debug rendering
		if (mDebugMode)
		{
			mDynamicsWorld->debugDrawWorld();
		}
	}

	bool cPhysicsWorld::AddComponent(iPhysicsComponent* component)
	{
		if (component)
		{
			auto concrete = dynamic_cast<cBulletWrapperComponent*>(component);
			if (concrete)
			{
				mDynamicsWorld->addRigidBody(concrete->mBody);
				components.push_back(component);
				return true;
			}
		}
		return false;
	}

	bool cPhysicsWorld::AddCharacter(iPhysicsComponent* component)
	{
		if (component)
		{
			auto concrete = dynamic_cast<cCharacterComponent*>(component);
			if (concrete)
			{
				mDynamicsWorld->addCollisionObject(
					concrete->mGhostObject,
					btBroadphaseProxy::CharacterFilter,
					btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter
					);
				mDynamicsWorld->addAction(concrete->mCharacterController);
				components.push_back(component);
				return true;
			}
		}
		return false;
	}

	bool cPhysicsWorld::AddObject(iPhysicsComponent* component)
	{
		if (component)
		{
			auto concrete = dynamic_cast<cTriggerRegionComponent*>(component);
			if (concrete)
			{
				mDynamicsWorld->addCollisionObject(concrete->mGhostObject);
				components.push_back(component);
				return true;
			}
		}
		return false;
	}

	bool cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
	{
		if (component)
		{
			auto concrete = dynamic_cast<cBulletWrapperComponent*>(component);
			if (concrete)
			{
				auto it = std::find(components.begin(), components.end(), component);
				if (it != components.end())
				{
					components.erase(it);
					mDynamicsWorld->removeRigidBody(concrete->mBody);
					return true;
				}
			}
		}
		return false;
	}

	bool cPhysicsWorld::RemoveCharacter(iPhysicsComponent* component)
	{
		if (component)
		{
			auto concrete = dynamic_cast<cCharacterComponent*>(component);
			if (concrete)
			{
				auto it = std::find(components.begin(), components.end(), component);
				if (it != components.end())
				{
					components.erase(it);
					mDynamicsWorld->removeCollisionObject(concrete->mGhostObject);
					mDynamicsWorld->removeCharacter(concrete->mCharacterController);
					return true;
				}
			}
		}
		return false;
	}

	bool cPhysicsWorld::RemoveObject(iPhysicsComponent* component)
	{
		if (component)
		{
			auto concrete = dynamic_cast<cTriggerRegionComponent*>(component);
			if (concrete)
			{
				auto it = std::find(components.begin(), components.end(), component);
				if (it != components.end())
				{
					components.erase(it);
					mDynamicsWorld->removeCollisionObject(concrete->mGhostObject);
					return true;
				}
			}
		}
		return false;
	}

	std::vector<RayCastResult> cPhysicsWorld::RayCastAll(vec3 ro, vec3 rd, float t)
	{
		std::vector<RayCastResult> hits;

		btVector3 from = nConvert::ToBullet(ro);
		btVector3 to = nConvert::ToBullet(ro + rd * t);

		btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
		allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
		//kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
		//allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
		allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;


		mDynamicsWorld->rayTest(from, to, allResults);

		for (unsigned int i = 0; i < allResults.m_collisionObjects.size(); ++i)
		{
			void* userpointer = allResults.m_collisionObjects[i]->getUserPointer();
			if (userpointer != 0)
			{
				RayCastResult result;
				result.object = (iPhysicsComponent*)userpointer;
				result.hitPoint = nConvert::ToGLM(allResults.m_hitPointWorld[i]);
				result.hitNormal = nConvert::ToGLM(allResults.m_hitNormalWorld[i]);
				result.distance = allResults.m_hitFractions[i];
				hits.push_back(result);
			}
		}

		return hits;
	}

	RayCastResult* cPhysicsWorld::RayCast(vec3 ro, vec3 rd, float t, iPhysicsComponent* ignoreLayer)
	{

		btVector3 from = nConvert::ToBullet(ro);
		btVector3 to = nConvert::ToBullet(ro + rd * t);

		btCollisionWorld::AllHitsRayResultCallback allResults(from, to);
		allResults.m_flags |= btTriangleRaycastCallback::kF_KeepUnflippedNormal;
		//kF_UseGjkConvexRaytest flag is now enabled by default, use the faster but more approximate algorithm
		//allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;
		allResults.m_flags |= btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;


		mDynamicsWorld->rayTest(from, to, allResults);

		if (!allResults.hasHit()) return nullptr;

		float min = FLT_MAX;
		int index = -1;

		for (unsigned int i = 0; i < allResults.m_collisionObjects.size(); ++i)
		{
			void* userpointer = allResults.m_collisionObjects[i]->getUserPointer();
			if (userpointer == ignoreLayer) continue;
			if (((iPhysicsComponent*)userpointer)->GetComponentType() == eComponentType::trigger) continue;
			if (userpointer != 0 && allResults.m_hitFractions[i] < min)
			{
				index = i;
				min = allResults.m_hitFractions[i];
			}
		}

		RayCastResult* result = 0;
		if (index != -1)
		{
			result = new RayCastResult;
			result->object = (iPhysicsComponent*)allResults.m_collisionObjects[index]->getUserPointer();
			result->hitPoint = nConvert::ToGLM(allResults.m_hitPointWorld[index]);
			result->hitNormal = nConvert::ToGLM(allResults.m_hitNormalWorld[index]);
			result->distance = min;
		}

		return result;
	}

	void cPhysicsWorld::SetDebugRenderer(iPhysicsDebugRenderer* renderer)
	{
		debugDrawer = new cPhysicsDebugDrawer(renderer);
		mDynamicsWorld->setDebugDrawer(debugDrawer);
		debugDrawer->DBG_DrawWireframe; // this breaks when I run the app
		debugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe); // so does this
		debugDrawer->setDebugMode(1); // this doesn't
	}

	void cPhysicsWorld::AddConstraint(btTypedConstraint* constraint)
	{
		this->mDynamicsWorld->addConstraint(constraint);
	}

}