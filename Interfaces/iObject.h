#pragma once

#include <interfaces/iComponent.h>
#include <sTransform.h>
#include <string>
#include <vector>

class iObject : public iSerializable
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
		for (iComponent* c : _components)
		{
			T* ptr = dynamic_cast<T*>(c);
			if (ptr != nullptr) return ptr;
		}
		T* ptr = new T(this);
		_components.push_back(ptr);
		return ptr;
	}

	iComponent* AddComponent(iComponent* component);
	

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

	const std::vector<iComponent*>& Components()
	{
		return _components;
	}

	virtual bool serialize(rapidxml::xml_node<>* root_node) override;
	
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

protected:

	virtual std::string ObjectName() = 0;

};