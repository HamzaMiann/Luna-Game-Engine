#include <Components/ComponentFactory.h>
#include <iObject.h>
#include <Behaviour/Controls/cPlayerBehaviour.h>
#include <Behaviour/Controls/cSphereBehaviour.h>
#include <Behaviour/AI/cSeekBehaviour.h>
#include <Behaviour/AI/cPursueBehaviour.h>
#include <Behaviour/AI/cWanderBehaviour.h>
#include <Behaviour/AI/cApproachBehaviour.h>
#include <Behaviour/Managers/cAIGameManager.h>

#pragma warning(disable)


iComponent* ComponentFactory::GetComponent(std::string type, iObject* object)
{
	if (type == "SphereBehaviour")
	{
		return (iComponent*)object->AddComponent<cSphereBehaviour>();
	}
	if (type == "PlayerBehaviour")
	{
		return (iComponent*)object->AddComponent<cPlayerBehaviour>();
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
	if (type == "AIApproachBehaviour")
	{
		return (iComponent*)object->AddComponent<AI::cApproachBehaviour>();
	}

	if (type == "AIGameManager")
	{
		return (iComponent*)object->AddComponent<cAIGameManager>();
	}
	
	
	return nullptr;
}