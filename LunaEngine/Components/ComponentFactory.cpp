#include <Components/ComponentFactory.h>
#include <Components/cRigidBody.h>
#include <iObject.h>

iComponent* ComponentFactory::GetComponent(std::string type, iObject* object)
{
	if (type == "cRigidBody")
	{
		return (iComponent*)object->AddComponent<cRigidBody>();
	}
	return nullptr;
}