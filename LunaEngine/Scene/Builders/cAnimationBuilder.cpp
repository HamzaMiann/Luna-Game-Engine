#include "cAnimationBuilder.h"
#include <Mesh/cModelLoader.h>
#include <iostream>

using namespace rapidxml;

void cAnimationBuilder::Build(Scene& scene, xml_node<>* node)
{
	for (xml_node<>* animation_node = node->first_node("Animation"); animation_node; animation_node = animation_node->next_sibling("Animation"))
	{
		std::string file = std::string("assets/models/") + animation_node->first_attribute("file")->value();
		std::string friendlyName = animation_node->first_attribute("friendlyName")->value();
		std::string mesh = animation_node->first_attribute("mesh")->value();
		if (!cModelLoader::Instance().LoadAnimation(file, friendlyName, mesh))
		{
			std::cout << "Unable to load animation: " << file << std::endl;
		}
	}
}
