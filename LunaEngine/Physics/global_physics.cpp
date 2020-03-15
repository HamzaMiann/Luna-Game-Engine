#include <Windows.h>
#include <interfaces/physics/iPhysicsFactory.h>
#include <iostream>
#include "global_physics.h"
typedef nPhysics::iPhysicsFactory* (*func_createPhysicsFactory)();

#define MY_PHYSICS

#ifdef MY_PHYSICS
#define LIBRARY_NAME "LunaPhysicsWrapper.dll"
#else
#define LIBRARY_NAME "BulletPhysicsWrapper.dll"
#endif

nPhysics::iPhysicsFactory* g_PhysicsFactory = 0;
nPhysics::iPhysicsWorld* g_PhysicsWorld = 0;

namespace _InitPhysics_
{
	bool initialized = false;
	HMODULE hModule = 0;
}

void InitPhysics()
{
	// only initialize once
	if (!_InitPhysics_::initialized)
	{
		// LOAD THE DLL
		_InitPhysics_::hModule = LoadLibrary(LIBRARY_NAME);
		// CHECK FOR ERRORS
		if (!_InitPhysics_::hModule)
		{
			std::cout << "Failed to load the physics library. Exiting..." << std::endl;
			exit(1);
		}

		// GET DLL FUNCTION
		func_createPhysicsFactory createFactory = 0;
		createFactory = (func_createPhysicsFactory)GetProcAddress(_InitPhysics_::hModule, "MakePhysicsFactory");
		// CHECK FOR ERRORS
		if (!createFactory)
		{
			std::cout << "Failed to get the DLL factory function. Exiting..." << std::endl;
			FreeLibrary(_InitPhysics_::hModule);
			exit(1);
		}

		// CREATE THE FACTORY
		g_PhysicsFactory = createFactory();
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
	if (_InitPhysics_::hModule != 0)
	{
		FreeLibrary(_InitPhysics_::hModule);
		_InitPhysics_::hModule = 0;
	}
}
