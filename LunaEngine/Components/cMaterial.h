#pragma once

#include <iObject.h>
#include <string>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <Texture/cTexture.h>
#include <Shader/Shader.h>
#include <Mesh/cVAOManager.h>

class cMaterial : public iComponent
{
public:
	cMaterial(iObject* obj);
	~cMaterial();

	sTransform& transform;
	iObject& parent;

	std::string layer;

	cTexture texture[4];
	Shader shader;
	sModelDrawInfo* mesh;

	glm::vec4 specColour; // w value is intensity

	float reflectivity;
	float refractivity;

	bool isUniformColour;
	bool isWireframe;
	bool isSkybox;

	void SetMesh(std::string meshName);


	// Inherited via iComponent
	virtual bool serialize(rapidxml::xml_node<>* root_node) override;

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

};