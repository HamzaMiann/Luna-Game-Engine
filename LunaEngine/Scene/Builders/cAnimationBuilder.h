#pragma once

#include "iSceneBuilder.h"

class cAnimationBuilder : public iSceneBuilder
{
public:
	cAnimationBuilder() {}
	virtual ~cAnimationBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};