#pragma once

#include <interfaces/physics/iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}

		virtual iPhysicsWorld* GetWorld() override;

		// components
		virtual iSphereComponent* CreateSphere(iObject* parent, const sSphereDef& def) override;
		virtual iPlaneComponent* CreatePlane(iObject* parent, const sPlaneDef& def) override;
	};
}