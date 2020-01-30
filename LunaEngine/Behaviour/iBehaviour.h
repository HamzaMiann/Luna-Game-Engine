#pragma once

#include <interfaces/iComponent.h>
#include <iObject.h>

class iBehaviour : public iComponent
{
	friend class cBehaviourManager;
public:
	virtual ~iBehaviour();
	virtual void start() = 0;
	virtual void update(float dt) = 0;
protected:
	iBehaviour(iObject* root);
	sTransform& transform;
	iObject& parent;
private:
	bool _initialized;
	iBehaviour() = delete;
};
