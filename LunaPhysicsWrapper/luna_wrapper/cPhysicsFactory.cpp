#include "cPhysicsFactory.h"
#include "cPhysicsComponent.h"

namespace nPhysics
{
	iSphereComponent* cPhysicsFactory::CreateBall(sSphereDef& definition, iObject* attach_to)
	{
		cSphereComponent* comp = new cSphereComponent(attach_to, definition);
		attach_to->AddComponent((iComponent*)comp);
		return comp;
	}

	iPlaneComponent* cPhysicsFactory::CreateBall(sPlaneDef& definition, iObject* attach_to)
	{
		return nullptr;
	}

	iPhysicsWorld* cPhysicsFactory::CreateWorld()
	{
		return nullptr;
	}
}
