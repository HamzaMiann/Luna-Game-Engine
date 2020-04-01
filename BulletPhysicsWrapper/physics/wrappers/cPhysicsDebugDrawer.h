#pragma once

#include "../bullet/LinearMath/btIDebugDraw.h"
#include <interfaces/physics/iPhysicsDebugRenderer.h>

namespace nPhysics
{
	class cPhysicsDebugDrawer : public btIDebugDraw
	{
		int mDebugMode;
		iPhysicsDebugRenderer* mRenderer;

	public:

		cPhysicsDebugDrawer(iPhysicsDebugRenderer* renderInstance);
		virtual ~cPhysicsDebugDrawer();

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

		virtual void drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar /*alpha*/) override;

		virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

		virtual void reportErrorWarning(const char* warningString) override;

		virtual void draw3dText(const btVector3& location, const char* textString) override;

		virtual void setDebugMode(int debugMode) override { mDebugMode = debugMode; }

		virtual int getDebugMode() const override { return mDebugMode; }

	};
}