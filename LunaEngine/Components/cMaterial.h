#pragma once

#include <iObject.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <string>

class cMaterial : public iComponent
{
public:
	cMaterial(iObject* obj) : transform(obj->transform) {}

	sTransform& transform;

	std::string meshName = "";
	std::string shaderName = "basic";

	glm::vec4  colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	glm::vec3 specColour = glm::vec3(.5f, .5f, .5f);

	float specIntensity = 1.f;

	bool uniformColour = false;
	bool isWireframe = false;
	bool isSkybox = false;

	std::string texture[4];
	float textureRatio[4];
};