#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include "cSphereComponent.h"
#include "cPlaneComponent.h"
#include <luna_physics/shapes.h>

namespace nPhysics
{
	iSphereComponent* cPhysicsFactory::CreateSphere(iObject* parent, const sSphereDef& def)
	{
		phys::iShape* shape = new phys::cSphere(def.Offset, def.Radius);
		phys::sRigidBodyDef df;
		df.Velocity = def.velocity;
		df.Mass = def.mass;
		df.Position = parent->transform.pos;
		return new cSphereComponent(parent, df, shape);
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(iObject* parent, const sPlaneDef& def)
	{
		phys::iShape* shape = new phys::cPlane(def.Normal, def.Constant);
		phys::sRigidBodyDef df;
		df.Velocity = def.velocity;
		df.Mass = def.mass;
		df.Position = parent->transform.pos;
		return new cPlaneComponent(parent, df, shape);
	}

	iPhysicsWorld* cPhysicsFactory::CreateWorld()
	{
		static cPhysicsWorld instance;
		return &instance;
	}
}