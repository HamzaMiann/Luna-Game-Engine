#include <Components/ComponentFactory.h>
#include <iObject.h>
#include <Behaviour/Controls/cSphereBehaviour.h>
#include <Behaviour/AI/cSeekBehaviour.h>
#include <Behaviour/AI/cPursueBehaviour.h>
#include <Behaviour/AI/cWanderBehaviour.h>

iComponent* ComponentFactory::GetComponent(std::string type, iObject* object)
{
	if (type == "SphereBehaviour")
	{
		return (iComponent*)object->AddComponent<cSphereBehaviour>();
	}
	if (type == "AISeekBehaviour")
	{
		return (iComponent*)object->AddComponent<AI::cSeekBehaviour>();
	}
	if (type == "AIPursueBehaviour")
	{
		return (iComponent*)object->AddComponent<AI::cPursueBehaviour>();
	}
	if (type == "AIWanderBehaviour")
	{
		return (iComponent*)object->AddComponent<AI::cWanderBehaviour>();
	}
	return nullptr;
}