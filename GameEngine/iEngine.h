#pragma once

#include <vector>

template<typename T>
class iEngine
{
protected:
	std::vector<T*> Components;
public:
	virtual ~iEngine() {}
	bool Register(T* component)
	{
		auto it = std::find(Components.begin(), Components.end(), component);
		if (it == Components.end())
		{
			Components.push_back(component);
			return true;
		}
		return false;
	}
	bool Unregister(T* component)
	{
		auto it = std::find(Components.begin(), Components.end(), component);
		if (it != Components.end())
		{
			Components.erase(it);
			return true;
		}
		return false;
	}
};