#pragma once


#include <interfaces/physics/iPhysicsFactory.h>


extern nPhysics::iPhysicsFactory* g_PhysicsFactory;
extern nPhysics::iPhysicsWorld* g_PhysicsWorld;

void InitPhysics();
void ReleasePhysics();