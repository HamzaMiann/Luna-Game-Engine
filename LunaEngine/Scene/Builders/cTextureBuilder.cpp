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

	cBasicTextureManager::Instance()->SetBasePath("assets/textures/cubemaps");
	for (xml_node<>* tex_node = node->first_node("Skybox"); tex_node; tex_node = tex_node->next_sibling("Skybox"))
	{
		std::string name = tex_node->first_attribute("name")->value();
		std::string right = tex_node->first_node("Right")->first_attribute("texture")->value();
		std::string left = tex_node->first_node("Left")->first_attribute("texture")->value();
		std::string top = tex_node->first_node("Top")->first_attribute("texture")->value();
		std::string bottom = tex_node->first_node("Bottom")->first_attribute("texture")->value();
		std::string front = tex_node->first_node("Front")->first_attribute("texture")->value();
		std::string back = tex_node->first_node("Back")->first_attribute("texture")->value();
		std::string error;
		if (right.substr(right.find_last_of(".") + 1) == "bmp")
		{
			if (!cBasicTextureManager::Instance()->CreateCubeTextureFromBMPFiles(name, right, left, top, bottom, front, back, true, error))
			{
				std::cout << "Didn't load skybox" << std::endl;
			}
		}
		else if (right.substr(right.find_last_of(".") + 1) == "png")
		{
			if (!cBasicTextureManager::Instance()->CreateCubeTextureFromPNGFiles(name, right, left, top, bottom, front, back, true, error))
			{
				std::cout << "Didn't load skybox" << std::endl;
			}
		}
		else if (right.substr(right.find_last_of(".") + 1) == "jpg")
		{
			if (!cBasicTextureManager::Instance()->CreateCubeTextureFromJPGFiles(name, right, left, top, bottom, front, back, true, error))
			{
				std::cout << "Didn't load skybox" << std::endl;
			}
		}
		else
		{
			std::cout << "Didn't load skybox (unsupported file type)." << std::endl;
		}
	}
}
