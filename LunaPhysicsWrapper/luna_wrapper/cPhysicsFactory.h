#pragma once

#include <interfaces/physics/iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}
		virtual iSphereComponent* CreateBall(sSphereDef& definition, iObject* attach_to);
		virtual iPlaneComponent* CreateBall(sPlaneDef& definition, iObject* attach_to);
		virtual iPhysicsWorld* CreateWorld();
	};
}