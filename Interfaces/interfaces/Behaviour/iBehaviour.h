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

	virtual void OnCollide(iObject* other) {}

	sTransform& transform;
	iObject& parent;

protected:
	iBehaviour(iObject* root);
	
private:
	bool _initialized;
	iBehaviour() = delete;
};
