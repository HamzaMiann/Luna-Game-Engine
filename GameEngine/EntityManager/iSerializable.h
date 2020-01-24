#pragma once

#include <rapidxml/rapidxml.hpp>

class iSerializable
{
public:
	virtual ~iSerializable() {}
	virtual bool serialize(rapidxml::xml_node<>* root_node) = 0;
	virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;
};