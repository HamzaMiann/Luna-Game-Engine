#pragma once

#include <iEngine.h>
#include <interfaces/Behaviour/iBehaviour.h>

class cBehaviourManager : public iEngine<iBehaviour>
{
private:
	cBehaviourManager();

public:
	virtual ~cBehaviourManager();

	static cBehaviourManager& Instance()
	{
		static cBehaviourManager instance;
		return instance;
	}

	virtual void start();
	virtual void update(float dt);

};