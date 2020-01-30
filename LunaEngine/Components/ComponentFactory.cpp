#include <Components/ComponentFactory.h>
#include <iObject.h>
#include <Behaviour/Controls/cSphereBehaviour.h>
#include <Behaviour/AI/cSeekBehaviour.h>

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
	return nullptr;
}