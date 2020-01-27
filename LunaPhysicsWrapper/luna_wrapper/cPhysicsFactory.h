#pragma once

#include <interfaces/physics/iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}
		virtual iSphereComponent* CreateBall(sSphereDef& definition);
		virtual iPlaneComponent* CreateBall(sPlaneDef& definition);
		virtual iPhysicsWorld* CreateWorld();
	};
}