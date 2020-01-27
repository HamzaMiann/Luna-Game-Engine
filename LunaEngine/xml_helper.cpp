#include "xml_helper.h"
#include <stdlib.h>

float XML_Helper::AsFloat(rapidxml::xml_node<>* node)
{
	return strtof(node->value(), 0);
}
glm::vec2 XML_Helper::AsVec2(rapidxml::xml_node<>* node)
{
	glm::vec2 vec;
	vec.x = strtof(node->first_attribute("x")->value(), 0);
	vec.y = strtof(node->first_attribute("y")->value(), 0);
	return vec;
}
glm::vec3 XML_Helper::AsVec3(rapidxml::xml_node<>* node)
{
	glm::vec3 vec;
	vec.x = strtof(node->first_attribute("x")->value(), 0);
	vec.y = strtof(node->first_attribute("y")->value(), 0);
	vec.z = strtof(node->first_attribute("z")->value(), 0);
	return vec;
}
glm::vec4 XML_Helper::AsVec4(rapidxml::xml_node<>* node)
{
	glm::vec4 vec;
	vec.x = strtof(node->first_attribute("x")->value(), 0);
	vec.y = strtof(node->first_attribute("y")->value(), 0);
	vec.z = strtof(node->first_attribute("z")->value(), 0);
	vec.w = strtof(node->first_attribute("w")->value(), 0);
	return vec;
}