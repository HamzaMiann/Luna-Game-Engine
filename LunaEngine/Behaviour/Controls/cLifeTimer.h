#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <EntityManager/cEntityManager.h>

class cLifeTimer : public iBehaviour
{
public:

	cLifeTimer(iObject* root) : iBehaviour(root) {}
	virtual ~cLifeTimer() {}

	virtual void start() {}

	virtual void update(float dt)
	{
		mTime -= dt;
		if (mTime < 0.f)
		{
			cEntityManager::Instance().RemoveEntity(&this->parent);
		}
	}

	float mTime = 5.f;

};