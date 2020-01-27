#pragma once

#include <iPhysicsFactory.h>

namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory() {}
		virtual iBallComponent* CreateBall(sBallDef& definition) {}
		virtual iPhysicsWorld* CreateWorld() {}
	};
}