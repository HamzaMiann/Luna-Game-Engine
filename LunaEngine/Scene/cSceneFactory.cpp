
#include "cSceneFactory.h"
#include "Builders/cAudioBuilder.h"
#include "Builders/cModelBuilder.h"
#include "Builders/cShaderBuilder.h"
#include "Builders/cLayoutBuilder.h"
#include "Builders/cLightBuilder.h"
#include "Builders/cTextureBuilder.h"

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
	if (builder_name == "Shaders")
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
	if (builder_name == "Textures")
	{
		return new cTextureBuilder();
	}
	return nullptr;
}