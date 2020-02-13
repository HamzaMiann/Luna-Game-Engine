#pragma once
#include "../nConvert.h"

class cDebugRenderer : public btIDebugDraw
{
public:
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

	virtual void reportErrorWarning(const char* warningString) override;

	virtual void draw3dText(const btVector3& location, const char* textString) override;

	virtual void setDebugMode(int debugMode) override;

	virtual int getDebugMode() const override;

private:
	//iDebugRenderer* mDebugRenderer;
};