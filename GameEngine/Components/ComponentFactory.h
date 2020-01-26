#pragma once

#include <string>
#include <Components/iComponent.h>
class iObject;

class ComponentFactory
{
public:
	ComponentFactory() = delete;
	static iComponent* GetComponent(std::string type, iObject* object);
};