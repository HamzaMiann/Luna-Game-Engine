#include "cTextureBuilder.h"
#include <Texture/cBasicTextureManager.h>
#include <iostream>
using namespace rapidxml;

void cTextureBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Loading Textures...\n");

	cBasicTextureManager::Instance()->SetBasePath("assets/textures");
	for (xml_node<>* tex_node = node->first_node("Texture"); tex_node; tex_node = tex_node->next_sibling("Texture"))
	{
		std::string texture = tex_node->first_attribute("name")->value();
		if (texture.substr(texture.find_last_of(".") + 1) == "bmp")
		{
			if (!cBasicTextureManager::Instance()->Create2DTextureFromBMPFile(texture, true))
			{
				std::cout << "Didn't load texture" << std::endl;
			}
		}
		else if (texture.substr(texture.find_last_of(".") + 1) == "png")
		{
			if (!cBasicTextureManager::Instance()->Create2DTextureFromPNGFile(texture, true))
			{
				std::cout << "Didn't load texture" << std::endl;
			}
		}
		else if (texture.substr(texture.find_last_of(".") + 1) == "jpg")
		{
			if (!cBasicTextureManager::Instance()->Create2DTextureFromJPGFile(texture, true))
			{
				std::cout << "Didn't load texture" << std::endl;
			}
		}
		else
		{
			std::cout << "Didn't load texture (unsupported file type)." << std::endl;
		}
	}
}
