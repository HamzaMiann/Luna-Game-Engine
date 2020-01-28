#include "cPhysicsFactory.h"
#include "cPhysicsComponent.h"
#include "cPhysicsWorld.h"

namespace nPhysics
{
	iSphereComponent* cPhysicsFactory::CreateSphere(const sSphereDef& def, iObject* attach_to)
	{
		cSphereComponent* comp = new cSphereComponent(attach_to, definition);
		attach_to->AddComponent((iComponent*)comp);
		return comp;
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(const sPlaneDef& def, iObject* attach_to)
	{
		return nullptr;
	}

	iPhysicsWorld* cPhysicsFactory::GetWorld()
	{
		static cPhysicsWorld world_instance;
		return &world_instance;
	}
}
