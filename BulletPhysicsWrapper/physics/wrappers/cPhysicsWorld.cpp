#include "cPhysicsWorld.h"
#include "cBulletWrapperComponent.h"
#include "../bullet/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h"
#include "../bullet/BulletCollision/Gimpact/btGImpactShape.h"
#include "../nConvert.h"

nPhysics::cPhysicsWorld::cPhysicsWorld()
{
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);

	mDynamicsWorld->setGravity(btVector3(0, -10, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
}

nPhysics::cPhysicsWorld::~cPhysicsWorld()
{
	// remove all components first
	auto it = components.begin();
	while (components.size() > 0) components.erase(it);

	delete mDynamicsWorld;
	delete mDispatcher;
	delete mCollisionConfiguration;
	delete mOverlappingPairCache;
	delete mSolver;
}

void nPhysics::cPhysicsWorld::Update(float dt)
{
	mDynamicsWorld->stepSimulation(dt, 10);
	//if (mCollisionListener)
	//{
	//	// TODO: COLLISION LISTENING STUFF
	//}
	for (unsigned int i = 0; i < mDispatcher->getNumManifolds(); ++i)
	{
		btPersistentManifold* manifold = mDispatcher->getManifoldByIndexInternal(i);
		iPhysicsComponent* bodyA = (iPhysicsComponent*)manifold->getBody0()->getUserPointer();
		iPhysicsComponent* bodyB = (iPhysicsComponent*)manifold->getBody1()->getUserPointer();
		bodyA->CollidedWith(bodyB);
		bodyB->CollidedWith(bodyA);
	}

	for (iPhysicsComponent*& i : components)
	{
		auto wrapper = dynamic_cast<cBulletWrapperComponent*>(i);
		i->UpdateTransform();
	}
}

bool nPhysics::cPhysicsWorld::AddComponent(iPhysicsComponent* component)
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

bool nPhysics::cPhysicsWorld::RemoveComponent(iPhysicsComponent* component)
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

std::vector<nPhysics::iPhysicsComponent*> nPhysics::cPhysicsWorld::RayCastAll(vec3 ro, vec3 rd, float t)
{
	std::vector<iPhysicsComponent*> hits;

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
			//iPhysicsComponent* body = reinterpret_cast<iPhysicsComponent*>(userpointer);
			iPhysicsComponent* body = (iPhysicsComponent*)userpointer;
			hits.push_back(body);
		}
	}

	return hits;
}

nPhysics::iPhysicsComponent* nPhysics::cPhysicsWorld::RayCast(vec3 ro, vec3 rd, float t)
{
	btVector3 from = nConvert::ToBullet(ro);
	btVector3 to = nConvert::ToBullet(ro + rd * t);

	btCollisionWorld::ClosestRayResultCallback closestResults(from, to);
	closestResults.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

	mDynamicsWorld->rayTest(from, to, closestResults);

	if (closestResults.hasHit())
	{
		void* userpointer = closestResults.m_collisionObject->getUserPointer();
		if (userpointer != 0)
		{
			return (iPhysicsComponent*)userpointer;
		}
	}

	return nullptr;
}
