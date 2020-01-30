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
		Components[i]->_initialized = true;
	}
}

void cBehaviourManager::update(float dt)
{
	for (size_t i = 0; i < Components.size(); ++i)
	{
		if (!Components[i]->_initialized)
		{
			Components[i]->start();
			Components[i]->_initialized = true;
		}
		Components[i]->update(dt);
	}
}
