#pragma once

#include <string>
#include <interfaces/iComponent.h>
class iObject;

class ComponentFactory
{
public:
	ComponentFactory() = delete;
	static iComponent* GetComponent(std::string type, iObject* object);
};