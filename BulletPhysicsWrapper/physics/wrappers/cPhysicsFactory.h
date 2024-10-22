#pragma once

#include <interfaces/physics/iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}

		/*
		return a concrete SINGLETON world
		*/
		virtual iPhysicsWorld* GetWorld() override;

		// components
		virtual iSphereComponent* CreateSphere(iObject* parent, const sSphereDef& def) override;
		virtual iPlaneComponent* CreatePlane(iObject* parent, const sPlaneDef& def) override;
		virtual iCubeComponent* CreateCube(iObject* parent, const sCubeDef& def) override;
		virtual iCapsuleComponent* CreateCapsule(iObject* parent, const sCapsuleDef& def) override;
		virtual iClothComponent* CreateCloth(iObject* parent, const sClothDef& def) override;
		virtual iPhysMeshComponent* CreateMesh(iObject* parent, const sMeshDef& def) override;
		virtual iCharacterComponent* CreateCharacter(iObject* parent, const sCharacterDef& def) override;
		virtual iTriggerRegionComponent* CreateTriggerRegion(iObject* parent, const sTriggerDef& def) override;
		virtual iEmptyComponent* CreateEmpty(iObject* parent) override;
	};
}

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory();