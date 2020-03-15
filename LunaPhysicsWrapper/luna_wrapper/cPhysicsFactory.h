#pragma once

#include <interfaces/physics/iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}

		virtual iPhysicsWorld* GetWorld() override;

		// implemented components
		virtual iSphereComponent* CreateSphere(iObject* parent, const sSphereDef& def) override;
		virtual iPlaneComponent* CreatePlane(iObject* parent, const sPlaneDef& def) override;
		virtual iClothComponent* CreateCloth(iObject* parent, const sClothDef& def) override;

		// not yet implemented
		virtual iCubeComponent* CreateCube(iObject* parent, const sCubeDef& def) override { return nullptr; }
		virtual iCapsuleComponent* CreateCapsule(iObject* parent, const sCapsuleDef& def) override { return nullptr; }
		virtual iEmptyComponent* CreateEmpty(iObject* parent) override { return nullptr; }

	};
}

#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory();