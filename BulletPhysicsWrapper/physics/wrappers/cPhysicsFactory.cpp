#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include "cSphereComponent.h"
#include "cPlaneComponent.h"
#include "cCubeComponent.h"
#include "cCapsuleComponent.h"
#include "cPhysMeshComponent.h"

namespace nPhysics
{
	iPhysicsWorld* cPhysicsFactory::GetWorld()
	{
		static cPhysicsWorld instance;
		return &instance;
	}

	iSphereComponent* cPhysicsFactory::CreateSphere(iObject* parent, const sSphereDef& def)
	{
		return new cSphereComponent(parent, def);
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(iObject* parent, const sPlaneDef& def)
	{
		return new cPlaneComponent(parent, def);
	}

	iCubeComponent* cPhysicsFactory::CreateCube(iObject* parent, const sCubeDef& def)
	{
		return new cCubeComponent(parent, def);
	}

	iClothComponent* cPhysicsFactory::CreateCloth(iObject* parent, const sClothDef& def)
	{
		return nullptr;
	}

	iPhysMeshComponent* cPhysicsFactory::CreateMesh(iObject* parent, const sMeshDef& def)
	{
		return new cPhysMeshComponent(parent, def);
	}

	iCapsuleComponent* cPhysicsFactory::CreateCapsule(iObject* parent, const sCapsuleDef& def)
	{
		return new cCapsuleComponent(parent, def);
	}

	iEmptyComponent* cPhysicsFactory::CreateEmpty(iObject* parent)
	{
		return nullptr;
	}

}

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory()
{
	return new nPhysics::cPhysicsFactory;
}
