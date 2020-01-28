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
		virtual iSphereComponent* CreateSphere(const sSphereDef& def, iObject* attach_to) = 0;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def, iObject* attach_to) = 0;
	};
}