
#include "cSceneFactory.h"
#include "cAudioBuilder.h"
#include "cModelBuilder.h"
#include "cShaderBuilder.h"
#include "cLayoutBuilder.h"
#include "cLightBuilder.h"

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
	if (builder_name == "Layout")
	{
		return new cLayoutBuilder();
	}
	if (builder_name == "Lights")
	{
		return new cLightBuilder();
	}
	return nullptr;
}