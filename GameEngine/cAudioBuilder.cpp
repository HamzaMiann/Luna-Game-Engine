
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
		if (file)
		{
			std::string friendlyName = "";
			if (friendly)
				friendlyName = friendly->value();
			scene.pAudioEngine->Create_Sound(file->value(), friendlyName);
		}
	}
}