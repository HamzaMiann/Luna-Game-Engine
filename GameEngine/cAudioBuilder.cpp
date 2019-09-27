
#include "cAudioBuilder.h"
using namespace rapidxml;

void cAudioBuilder::Build(Scene& scene, xml_node<>* node)
{
	scene.pAudioEngine = new AudioEngine();
	scene.pAudioEngine->Init();

	for (xml_node<>* sound_node = node->first_node("Sound"); sound_node; sound_node = sound_node->next_sibling("Sound"))
	{
		xml_attribute<>* file = sound_node->first_attribute("file");
		xml_attribute<>* friendly = sound_node->first_attribute("friendlyName");
		xml_attribute<>* streamedAttr = sound_node->first_attribute("streamed");
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
			AUDIO_ID id = scene.pAudioEngine->Create_Sound(file->value(), friendlyName, streamed);
			AudioEngine::Sound* sound = scene.pAudioEngine->GetSound(id);
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