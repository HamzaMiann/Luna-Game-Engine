#include "cPhysicsFactory.h"
#include "cSphereComponent.h"
#include "cPhysicsWorld.h"

namespace nPhysics
{
	iSphereComponent* cPhysicsFactory::CreateSphere(const sSphereDef& def, iObject* attach_to)
	{
		phys::sRigidBodyDef bodyDef;
		bodyDef.Mass = def.Mass;
		bodyDef.Position = attach_to->transform.pos;
		bodyDef.Velocity = glm::vec3(0.f);
		cSphereComponent* comp = new cSphereComponent(attach_to, bodyDef, def);
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
