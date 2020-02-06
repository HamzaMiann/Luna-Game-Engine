#include "cMaterial.h"

cMaterial::cMaterial(iObject* obj) :
	transform(obj->transform),
	parent(*obj),
	mesh(0),
	specColour(glm::vec4(.5f, .5f, .5f, 1.0f)),
	isUniformColour(false),
	isWireframe(false),
	isSkybox(false),
	reflectivity(0.f),
	refractivity(0.f),
	layer("default")
{
}

cMaterial::~cMaterial()
{
}

void cMaterial::SetMesh(std::string meshName)
{
	sModelDrawInfo info;
	if (cVAOManager::Instance()->FindDrawInfoByModelName(meshName, info))
	{
		mesh = &info;
	}
	else
	{
		mesh = 0;
	}
}

bool cMaterial::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cMaterial::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}
