#pragma once

#include <Components/iComponent.h>
#include <sTransform.h>
#include <vector>

class iObject
{
private:
	std::vector<iComponent*> _components;

public:

	sTransform transform;

	virtual ~iObject()
	{
		for (unsigned int i = 0; i < _components.size(); ++i)
		{
			delete _components[i];
		}
		_components.clear();
	}

	template<typename T>
	T* AddComponent()
	{
		T* ptr = new T(this);
		_components.push_back(ptr);
		return ptr;
	}

	template<typename T>
	T* GetComponent()
	{
		for (iComponent* c : _components)
		{
			T* ptr = dynamic_cast<T*>(c);
			if (ptr != nullptr) return ptr;
		}
		return nullptr;
	}

	std::vector<iComponent*>& Components()
	{
		return _components;
	}

};