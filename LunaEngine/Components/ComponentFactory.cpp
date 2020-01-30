#include <Components/ComponentFactory.h>
#include <iObject.h>
#include <Behaviour/Controls/cSphereBehaviour.h>

iComponent* ComponentFactory::GetComponent(std::string type, iObject* object)
{
	if (type == "SphereBehaviour")
	{
		return (iComponent*)object->AddComponent<cSphereBehaviour>();
	}
	return nullptr;
}