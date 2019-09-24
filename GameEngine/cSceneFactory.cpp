
#include "cSceneFactory.h"
#include "cAudioBuilder.h"
#include "cModelBuilder.h"
#include "cShaderBuilder.h"

iSceneBuilder* cSceneFactory::GetBuilder(std::string builder_name)
{
	if (builder_name == "Audio")
	{
		return new cAudioBuilder();
	}
	if (builder_name == "Models")
	{
		return new cModelBuilder();
	}
	if (builder_name == "Shader")
	{
		return new cShaderBuilder();
	}
	return nullptr;
}