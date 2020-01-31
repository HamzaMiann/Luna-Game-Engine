#include "global_physics.h"

#define MY_PHYSICS

#ifdef MY_PHYSICS
#include <luna_wrapper/cPhysicsFactory.h>
#pragma comment(lib, "LunaPhysicsWrapper.lib")
typedef nPhysics::cPhysicsFactory MyPhysicsFactoryClass;
#endif

nPhysics::iPhysicsFactory* g_PhysicsFactory = 0;
nPhysics::iPhysicsWorld* g_PhysicsWorld = 0;

namespace _InitPhysics_
{
	bool initialized = false;
}

void InitPhysics()
{
	// only initialize once
	if (!_InitPhysics_::initialized)
	{
		g_PhysicsFactory = new MyPhysicsFactoryClass();
		g_PhysicsWorld = g_PhysicsFactory->GetWorld();
		_InitPhysics_::initialized = true;
	}
}

void ReleasePhysics()
{
	if (g_PhysicsWorld != 0)
	{
		//delete g_PhysicsWorld;
		// pointer to static object, no need to delete
		g_PhysicsWorld = 0;
	}
	if (g_PhysicsFactory != 0)
	{
		delete g_PhysicsFactory;
		g_PhysicsFactory = 0;
	}
}
