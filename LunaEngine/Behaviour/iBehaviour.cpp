#include <interfaces/Behaviour/iBehaviour.h>
#include <Behaviour/cBehaviourManager.h>

iBehaviour::iBehaviour(iObject* root, ComponentType type)
	: iComponent(type)
	, parent(*root)
	, transform(root->transform)
	, _initialized(false)
{
	cBehaviourManager::Instance().Register(this);
}

iBehaviour::iBehaviour(iObject* root)
	: iComponent(ComponentType::Behaviour)
	, parent(*root)
	, transform(root->transform)
	, _initialized(false)
{
	cBehaviourManager::Instance().Register(this);
}

iBehaviour::~iBehaviour()
{
	cBehaviourManager::Instance().Unregister(this);
}
