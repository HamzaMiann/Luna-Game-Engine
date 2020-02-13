#include "cPhysicsWorld.h"
#include "cBulletWrapperComponent.h"

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
	for (iPhysicsComponent*& i : components)
	{
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
