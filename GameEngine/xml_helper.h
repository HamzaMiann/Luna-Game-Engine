#pragma once

#include <rapidxml/rapidxml.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class XML_Helper
{
public:
	static float AsFloat(rapidxml::xml_node<>* node);
	static glm::vec2 AsVec2(rapidxml::xml_node<>* node);
	static glm::vec3 AsVec3(rapidxml::xml_node<>* node);
	static glm::vec4 AsVec4(rapidxml::xml_node<>* node);
};
