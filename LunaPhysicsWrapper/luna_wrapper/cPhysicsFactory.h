#pragma once

#include <interfaces/physics/iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}
		virtual iSphereComponent* CreateSphere(const sSphereDef& def, iObject* attach_to) override;
		virtual iPlaneComponent* CreatePlane(const sPlaneDef& def, iObject* attach_to) override;
		virtual iPhysicsWorld* GetWorld() override;
	};
}