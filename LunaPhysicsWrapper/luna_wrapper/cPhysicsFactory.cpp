#include "cPhysicsFactory.h"
#include "cPhysicsComponent.h"

namespace nPhysics
{
	iSphereComponent* cPhysicsFactory::CreateBall(sSphereDef& definition)
	{
		//cSphereComponent* comp = new cSphereComponent;
		return nullptr;
	}

	iPlaneComponent* cPhysicsFactory::CreateBall(sPlaneDef& definition)
	{
		return nullptr;
	}

	iPhysicsWorld* cPhysicsFactory::CreateWorld()
	{
		return nullptr;
	}
}
