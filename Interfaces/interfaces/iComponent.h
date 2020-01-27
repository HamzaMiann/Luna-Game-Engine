#pragma once

#include <interfaces/iSerializable.h>

class iComponent : public iSerializable
{
public:
	
	iComponent() {}
	virtual ~iComponent() {}

	virtual bool serialize(rapidxml::xml_node<>* root_node) = 0;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;

};