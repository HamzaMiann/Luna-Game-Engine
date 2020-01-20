#pragma once

#include <map>
class iComponent;

class iEngine
{
protected:
	std::map<iComponent*, iComponent*> components;
public:
	virtual ~iEngine() {}
	void Register(iComponent* component)
	{
		components[component] = component;
	}
	void Unregister(iComponent* component)
	{
		components[component] = 0;
	}
};