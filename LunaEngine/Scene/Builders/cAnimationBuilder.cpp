#include "cAnimationBuilder.h"
#include <Animation/cAnimationManager.h>
#include <iostream>
#include <Animation/cSimpleAssimpSkinnedMeshLoader_OneMesh.h>

using namespace rapidxml;

void cAnimationBuilder::Build(Scene& scene, xml_node<>* node)
{
	for (xml_node<>* profile_node = node->first_node("AnimationProfile"); profile_node; profile_node = profile_node->next_sibling("AnimationProfile"))
	{
		std::string name = profile_node->first_attribute("profile")->value();
		std::string mesh = profile_node->first_attribute("mesh")->value();
		cSimpleAssimpSkinnedMesh* profile = new cSimpleAssimpSkinnedMesh;
		profile->LoadMeshFromFile(name, "assets/models/" + mesh);

		for (xml_node<>* animation_node = profile_node->first_node("Animation"); animation_node; animation_node = animation_node->next_sibling("Animation"))
		{
			std::string file = animation_node->first_attribute("file")->value();
			std::string friendlyName = animation_node->first_attribute("friendlyName")->value();
			profile->LoadMeshAnimation(friendlyName, "assets/models/" + file);
		}
		
		cAnimationManager::Instance().profiles[name] = profile;

		/*if (!cModelLoader::Instance().LoadAnimation(file, friendlyName, mesh))
		{
			std::cout << "Unable to load animation: " << file << std::endl;
		}*/
	}
}
