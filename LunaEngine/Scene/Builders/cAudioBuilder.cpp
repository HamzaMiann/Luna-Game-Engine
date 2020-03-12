
#include "cAudioBuilder.h"
#include <Audio/AudioEngine.hpp>
#include <cGameObject.h>
#include <EntityManager/cEntityManager.h>
using namespace rapidxml;

void cAudioBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Building Audio Engine...\n");

	auto& vecGameObjects = cEntityManager::Instance().GetEntities();
	auto pAudioEngine = AudioEngine::Instance();

	pAudioEngine->Init();

	for (xml_node<>* sound_node = node->first_node(); sound_node; sound_node = sound_node->next_sibling())
	{
		std::string node_name = sound_node->name();
		if (node_name != "Sound" && node_name != "Sound3D") continue;

		xml_attribute<>* file = sound_node->first_attribute("file");
		xml_attribute<>* friendly = sound_node->first_attribute("friendlyName");
		xml_attribute<>* streamedAttr = sound_node->first_attribute("streamed");
		xml_attribute<>* attachAttr = sound_node->first_attribute("attach");
		xml_attribute<>* groupAttr = sound_node->first_attribute("group");
		if (file)
		{
			bool streamed = false;
			std::string friendlyName = "";
			if (friendly)
				friendlyName = friendly->value();
			if (streamedAttr)
			{
				std::string value = streamedAttr->value();
				if (value == "true")
					streamed = true;
			}
			AUDIO_ID id;
			if (node_name == "Sound")
			{
				if (!groupAttr)
				{
					id = pAudioEngine->Create_Sound(file->value(), friendlyName, streamed);
				}
				else
				{
					id = pAudioEngine->Create_Sound_Group(file->value(), friendlyName, groupAttr->value());
				}
			}
			else if (node_name == "Sound3D")
			{
				std::string tag = attachAttr->value();
				bool found = false;
				for (unsigned int i = 0; i < vecGameObjects.size(); ++i)
				{
					if (vecGameObjects[i]->tag == tag)
					{
						id = pAudioEngine->Create_Sound_3d(file->value(), friendlyName, vecGameObjects[i]);
						found = true;
						break;
					}
				}
				if (!found) continue;
			}
			else continue;
			AudioEngine::Sound* sound = pAudioEngine->GetSound(id);
			if (sound)
			{
				for (xml_attribute<>* attribute = sound_node->first_attribute();
					 attribute; attribute = attribute->next_attribute())
				{
					std::string value = attribute->value();
					std::string name = attribute->name();
					if (name == "volume")
					{
						sound->set_volume(strtof(value.c_str(), 0));
					}
					else if (name == "pitch")
					{
						sound->set_pitch(strtof(value.c_str(), 0));
					}
					else if (name == "pan")
					{
						sound->set_pan(strtof(value.c_str(), 0));
					}
				}
			}
		}
	}
}