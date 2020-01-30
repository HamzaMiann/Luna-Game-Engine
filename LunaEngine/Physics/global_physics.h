#pragma once

#define MY_PHYSICS

#include <interfaces/physics/iPhysicsFactory.h>

#ifdef MY_PHYSICS
#include <luna_wrapper/cPhysicsFactory.h>
#pragma comment(lib, "LunaPhysicsWrapper.lib")
typedef nPhysics::cPhysicsFactory MyPhysicsFactoryClass;
#endif

extern nPhysics::iPhysicsFactory* g_PhysicsFactory;
extern nPhysics::iPhysicsWorld* g_PhysicsWorld;

void InitPhysics();
void ReleasePhysics();