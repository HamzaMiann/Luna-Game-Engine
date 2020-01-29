#pragma once

#include <rapidxml/rapidxml.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <stdlib.h>


class XML_Helper
{
public:
	static inline float AsFloat(rapidxml::xml_node<>* node)
	{
		return strtof(node->value(), 0);
	}
	static glm::vec2 AsVec2(rapidxml::xml_node<>* node)
	{
		glm::vec2 vec;
		vec.x = strtof(node->first_attribute("x")->value(), 0);
		vec.y = strtof(node->first_attribute("y")->value(), 0);
		return vec;
	}
	static glm::vec3 AsVec3(rapidxml::xml_node<>* node)
	{
		glm::vec3 vec;
		vec.x = strtof(node->first_attribute("x")->value(), 0);
		vec.y = strtof(node->first_attribute("y")->value(), 0);
		vec.z = strtof(node->first_attribute("z")->value(), 0);
		return vec;
	}
	static glm::vec4 AsVec4(rapidxml::xml_node<>* node)
	{
		glm::vec4 vec;
		vec.x = strtof(node->first_attribute("x")->value(), 0);
		vec.y = strtof(node->first_attribute("y")->value(), 0);
		vec.z = strtof(node->first_attribute("z")->value(), 0);
		vec.w = strtof(node->first_attribute("w")->value(), 0);
		return vec;
	}
};
