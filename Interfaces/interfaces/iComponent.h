#pragma once

#include <interfaces/iSerializable.h>

enum class ComponentType
{
	Physics,
	Rendering,
	Animation,
	Behaviour,
	Other
};

class iComponent : public iSerializable
{
public:
	
	virtual ~iComponent() {}

	virtual bool serialize(rapidxml::xml_node<>* root_node) = 0;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;

	inline const ComponentType& GetComponentType() { return _type; }
	inline bool IsType(const ComponentType& type) { return type == _type; }

protected:
	ComponentType _type;

	iComponent() :
		_type(ComponentType::Other)
	{
	}

	iComponent(ComponentType type) :
		_type(type)
	{
	}
};