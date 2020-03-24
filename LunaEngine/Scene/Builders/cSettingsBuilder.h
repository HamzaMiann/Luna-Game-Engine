#pragma once

#include "iSceneBuilder.h"

class cSettingsBuilder : public iSceneBuilder
{
public:
	cSettingsBuilder() {}
	virtual ~cSettingsBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};