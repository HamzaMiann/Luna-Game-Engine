#pragma once

#include <interfaces/iComponent.h>
#include <sTransform.h>
#include <string>
#include <vector>

#define SAFE_DELETE(x) if (x) { delete x; x = 0; }

class iObject : public iSerializable
{
private:
	std::vector<iComponent*> _components;

public:

	sTransform transform;
	std::string tag = "";

	iObject* parent = 0;
	std::vector<iObject*> Children;

	inline void AddChild(iObject* child) { Children.push_back(child); }
	inline void RemoveChild(iObject* child) { Children.erase(std::find(Children.begin(), Children.end(), child)); }

	virtual ~iObject()
	{
		for (unsigned int i = 0; i < _components.size(); ++i)
		{
			SAFE_DELETE(_components[i])
		}
		_components.clear();
	}

	mat4 TranslationMatrix()
	{
		if (parent)
		{
			return parent->TranslationMatrix()* parent->RotationMatrix() * transform.TranslationMatrix();
		}
		else return transform.ModelMatrix();
	}

	mat4 RotationMatrix()
	{
		if (parent)
		{
			return parent->RotationMatrix() * transform.RotationMatrix();
		}
		else return transform.RotationMatrix();
	}

	mat4 ScaleMatrix()
	{
		if (parent)
		{
			return parent->ScaleMatrix() * transform.ScaleMatrix();
		}
		else return transform.ScaleMatrix();
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

	template<typename T>
	void RemoveComponent()
	{
		auto it = _components.begin();
		while (it != _components.end())
		{
			iComponent* comp = *it;
			T* ptr = dynamic_cast<T*>(comp);
			if (ptr != nullptr)
			{
				SAFE_DELETE(*it)
				_components.erase(it);
				return;
			}
			it++;
		}
	}

	inline const std::vector<iComponent*>& Components()
	{
		return _components;
	}

	virtual bool serialize(rapidxml::xml_node<>* root_node) override;
	
	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

protected:

	virtual std::string ObjectName() = 0;

};