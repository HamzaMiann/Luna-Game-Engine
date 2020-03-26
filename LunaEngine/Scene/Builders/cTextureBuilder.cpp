#include "cTextureBuilder.h"
#include <Texture/cBasicTextureManager.h>
#include <iostream>
#include <threading.h>
using namespace rapidxml;

#define THREADED

void LoadTextures(std::vector<std::string> texturesToLoad)
{
	cBasicTextureManager* manager = cBasicTextureManager::Instance();
	for (std::string texture : texturesToLoad)
	{
		sTextureData* data = new sTextureData;
		if (texture.substr(texture.find_last_of(".") + 1) == "png")
		{
			if (!manager->LoadPNGFromFile("assets/textures/" + texture, *data))
			{
				std::cout << "Didn't load texture" << std::endl;
			}
			else
			{
#ifdef THREADED
				Thread::Dispatch([texture, data]()
					{
						cBasicTextureManager::Instance()->Create2DTextureRGBA(texture, *data, true);
						delete data;
					}
				);
				//std::this_thread::sleep_for(std::chrono::duration<float>(0.1f));
#else
				cBasicTextureManager::Instance()->Create2DTextureRGBA(texture, *data, true);
				delete data; data = 0;
#endif
			}
		}
		else if (texture.substr(texture.find_last_of(".") + 1) == "jpg")
		{
			if (!manager->LoadJPGFromFile("assets/textures/" + texture, *data))
			{
				std::cout << "Didn't load texture" << std::endl;
			}
			else
			{
#ifdef THREADED
				Thread::Dispatch([texture, data]()
					{
						cBasicTextureManager::Instance()->Create2DTextureRGB(texture, *data, true);
						delete data;
					}
				);
				//std::this_thread::sleep_for(std::chrono::duration<float>(0.1f));
#else
				cBasicTextureManager::Instance()->Create2DTextureRGB(texture, *data, true);
				delete data; data = 0;
#endif
			}
		}
		else
		{
			std::cout << "Didn't load texture (unsupported file type)." << std::endl;
		}
	}
}

std::vector<std::string> texturesToLoad;

void cTextureBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Loading Textures...\n");

	cBasicTextureManager::Instance()->SetBasePath("assets/textures/cubemaps");
	for (xml_node<>* tex_node = node->first_node("Skybox"); tex_node; tex_node = tex_node->next_sibling("Skybox"))
	{
		LoadSkyboxTextures(tex_node);
	}

	for (xml_node<>* tex_node = node->first_node("Texture"); tex_node; tex_node = tex_node->next_sibling("Texture"))
	{
		texturesToLoad.push_back(tex_node->first_attribute("name")->value());
	}

	cBasicTextureManager::Instance()->SetBasePath("assets/textures");
#ifdef THREADED
	(new std::thread(LoadTextures, texturesToLoad))->detach();
#else
	LoadTextures(texturesToLoad);
#endif
}

void cTextureBuilder::LoadSkyboxTextures(rapidxml::xml_node<>* tex_node)
{
	cBasicTextureManager* manager = cBasicTextureManager::Instance();

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
		if (!manager->CreateCubeTextureFromBMPFiles(name, right, left, top, bottom, front, back, true, error))
		{
			std::cout << "Didn't load skybox" << std::endl;
		}
	}
	else if (right.substr(right.find_last_of(".") + 1) == "png")
	{
		if (!manager->CreateCubeTextureFromPNGFiles(name, right, left, top, bottom, front, back, true, error))
		{
			std::cout << "Didn't load skybox" << std::endl;
		}
	}
	else if (right.substr(right.find_last_of(".") + 1) == "jpg")
	{
		if (!manager->CreateCubeTextureFromJPGFiles(name, right, left, top, bottom, front, back, true, error))
		{
			std::cout << "Didn't load skybox" << std::endl;
		}
	}
	else
	{
		std::cout << "Didn't load skybox (unsupported file type)." << std::endl;
	}
}
