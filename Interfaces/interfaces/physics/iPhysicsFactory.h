#pragma once
#include "iSphereComponent.h"
#include "iPlaneComponent.h"
#include "iPhysicsWorld.h"

namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {}
		virtual iPhysicsWorld* CreateWorld() = 0;

		// components
		virtual iSphereComponent* CreateSphere(const sSphereDef& def) = 0;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def) = 0;
	};
}