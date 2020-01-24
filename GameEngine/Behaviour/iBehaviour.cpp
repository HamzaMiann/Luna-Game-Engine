#include "iBehaviour.h"
#include <Behaviour/cBehaviourManager.h>

iBehaviour::iBehaviour()
{
	cBehaviourManager::Instance()->Register(this);
}

iBehaviour::~iBehaviour()
{
	cBehaviourManager::Instance()->Unregister(this);
}
