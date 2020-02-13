#pragma once
#include "iSphereComponent.h"
#include "iPlaneComponent.h"
#include "iCubeComponent.h"
#include "iCapsuleComponent.h"
#include "iEmptyComponent.h"
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
		virtual iCubeComponent* CreateCube(iObject* parent, const sCubeDef& def) = 0;
		virtual iCapsuleComponent* CreateCapsule(iObject* parent, const sCapsuleDef& def) = 0;
		virtual iEmptyComponent* CreateEmpty(iObject* parent) = 0;
	};
}