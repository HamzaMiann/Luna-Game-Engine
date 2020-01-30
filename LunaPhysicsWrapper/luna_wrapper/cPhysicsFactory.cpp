#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include "cSphereComponent.h"
#include "cPlaneComponent.h"
#include <luna_physics/shapes.h>

namespace nPhysics
{
	iPhysicsWorld* cPhysicsFactory::GetWorld()
	{
		static cPhysicsWorld instance;
		return &instance;// new cPhysicsWorld;
	}

	iSphereComponent* cPhysicsFactory::CreateSphere(iObject* parent, const sSphereDef& def)
	{
		phys::iShape* shape = new phys::cSphere(def.Offset, def.Radius);
		phys::sRigidBodyDef df;
		df.Velocity = def.velocity;
		df.Mass = def.mass;
		df.Position = parent->transform.pos;
		df.GravityFactor = def.gravity_factor;
		return new cSphereComponent(parent, df, shape);
	}

	iPlaneComponent* cPhysicsFactory::CreatePlane(iObject* parent, const sPlaneDef& def)
	{
		phys::iShape* shape = new phys::cPlane(def.Normal, def.Constant);
		phys::sRigidBodyDef df;
		df.Velocity = def.velocity;
		df.Mass = def.mass;
		df.Position = parent->transform.pos;
		df.GravityFactor = def.gravity_factor;
		return new cPlaneComponent(parent, df, shape);
	}

}