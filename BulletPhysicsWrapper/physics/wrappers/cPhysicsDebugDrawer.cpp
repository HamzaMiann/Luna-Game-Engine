#include "cPhysicsDebugDrawer.h"
#include "../nConvert.h"

namespace nPhysics {

	cPhysicsDebugDrawer::cPhysicsDebugDrawer(iPhysicsDebugRenderer* renderInstance)
	{
		mRenderer = renderInstance;
	}

	cPhysicsDebugDrawer::~cPhysicsDebugDrawer()
	{
	}

	void cPhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		mRenderer->DrawLine(nConvert::ToGLM(from), nConvert::ToGLM(to), nConvert::ToGLM(color));
	}

	void cPhysicsDebugDrawer::drawTriangle(const btVector3& v0, const btVector3& v1, const btVector3& v2, const btVector3& color, btScalar)
	{
		mRenderer->DrawTriangle(nConvert::ToGLM(v0), nConvert::ToGLM(v1), nConvert::ToGLM(v2), nConvert::ToGLM(color));
	}

	void cPhysicsDebugDrawer::drawSphere(btScalar radius, const btTransform& transform, const btVector3& color)
	{
		//mRenderer->DrawSphere(nConvert::ToGLM(transform.getOrigin()), radius, nConvert::ToGLM(color));
		mRenderer->DrawSphere(nConvert::ToGLM(transform), radius, nConvert::ToGLM(color));
	}

	void cPhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
	}

	void cPhysicsDebugDrawer::reportErrorWarning(const char* warningString)
	{
	}

	void cPhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString)
	{
	}


}