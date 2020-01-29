#pragma once

#include <interfaces/iComponent.h>
#include <iObject.h>

class iBehaviour : public iComponent
{
public:
	iBehaviour(iObject* root);
	virtual ~iBehaviour();
	virtual void start() = 0;
	virtual void update(float dt) = 0;
protected:
	sTransform& transform;
	iObject& parent;
};