#include "cBehaviourManager.h"

cBehaviourManager::cBehaviourManager()
{
}

cBehaviourManager::~cBehaviourManager()
{
}

void cBehaviourManager::start()
{
	for (size_t i = 0; i < Components.size(); ++i)
	{
		Components[i]->start();
	}
}

void cBehaviourManager::update(float dt)
{
	for (size_t i = 0; i < Components.size(); ++i)
	{
		Components[i]->update(dt);
	}
}
