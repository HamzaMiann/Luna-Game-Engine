#pragma once
#include "iSphereComponent.h"
#include "iPlaneComponent.h"
#include "iCubeComponent.h"
#include "iCapsuleComponent.h"
#include "iEmptyComponent.h"
#include "iClothComponent.h"
#include "iPhysicsWorld.h"
#include "iPhysMeshComponent.h"
#include "iCharacterComponent.h"
#include "iTriggerRegionComponent.h"

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
		virtual iPhysMeshComponent* CreateMesh(iObject* parent, const sMeshDef& def) = 0;
		virtual iClothComponent* CreateCloth(iObject* parent, const sClothDef& def) = 0;
		virtual iCharacterComponent* CreateCharacter(iObject* parent, const sCharacterDef& def) = 0;
		virtual iTriggerRegionComponent* CreateTriggerRegion(iObject* parent, const sTriggerDef& def) = 0;
		virtual iEmptyComponent* CreateEmpty(iObject* parent) = 0;
	};
}