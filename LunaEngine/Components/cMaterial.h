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

	std::string layer;

	sTransform& transform;
	iObject& parent;

	cTexture texture[4];
	Shader* shader;

	sModelDrawInfo* drawInfo;
	cMesh* mesh;

	glm::vec4 specColour;	// w value is intensity

	float reflectivity;		// 0 to 1
	float refractivity;		// 0 to 1 overwrites reflectivity

	bool isUniformColour;
	bool isWireframe;
	bool isSkybox;

	inline bool HasMesh() { return mesh != 0; }
	void SetMesh(std::string meshName);


	// Inherited via iComponent
	virtual bool serialize(rapidxml::xml_node<>* root_node) override;

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

};