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
		virtual iPhysicsWorld* GetWorld() = 0;
		// components
		virtual iSphereComponent* CreateSphere(iObject* parent, const sSphereDef& def) = 0;
		virtual iPlaneComponent* CreatePlane(iObject* parent, const sPlaneDef& def) = 0;
	};
}