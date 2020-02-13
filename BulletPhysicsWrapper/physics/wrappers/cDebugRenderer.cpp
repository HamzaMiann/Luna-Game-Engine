#include "cDebugRenderer.h"

void cDebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	//mDebugRenderer->DrawLine(nConvert::ToGLM(from), nConvert::ToGLM(to), nConvert::ToGLM(color));
}

void cDebugRenderer::reportErrorWarning(const char* warningString)
{
}

void cDebugRenderer::draw3dText(const btVector3& location, const char* textString)
{
}

void cDebugRenderer::setDebugMode(int debugMode)
{
}

int cDebugRenderer::getDebugMode() const
{
	return 0;
}
