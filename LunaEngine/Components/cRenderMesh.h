#pragma once

#include <iObject.h>
#include <Components/iRenderMesh.h>
#include <Texture/cTexture.h>
#include <Shader/Shader.h>
#include <Mesh/cVAOManager.h>

class cRenderMesh : public iRenderMesh
{
public:
	cRenderMesh(iObject* obj);
	virtual ~cRenderMesh();

	
	class cMeshData {
		friend class cRenderMesh;
		sModelDrawInfo* drawInfo;
		cMesh* mesh;
	public:
		cMeshData():
			mesh(0),
			drawInfo(0)
		{ }
		inline bool HasMesh() { return mesh == 0; }
		void SetMesh(std::string meshName);
	} mesh_data;


	cTexture texture[4];
	Shader* shader;

	vec4 specColour;	// w value is intensity

	float reflectivity;		// 0 to 1
	float refractivity;		// 0 to 1 overwrites reflectivity

	bool isUniformColour;
	bool isWireframe;
	bool isSkybox;

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

private:
	virtual void _render() override;

	virtual void _set_textures(Shader& shader);
	void _render_mesh(Shader& shader, sModelDrawInfo& mesh);
};