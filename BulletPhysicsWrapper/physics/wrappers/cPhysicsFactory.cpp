#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include "cSphereComponent.h"
#include "cPlaneComponent.h"

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

}

DLL_EXPORT nPhysics::iPhysicsFactory* MakePhysicsFactory()
{
	return new nPhysics::cPhysicsFactory;
}
