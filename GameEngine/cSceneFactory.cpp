
#include "cSceneFactory.h"
#include "cAudioBuilder.h"

iSceneBuilder* cSceneFactory::GetBuilder(std::string builder_name)
{
	if (builder_name == "Audio")
	{
		return new cAudioBuilder();
	}
	return nullptr;
}