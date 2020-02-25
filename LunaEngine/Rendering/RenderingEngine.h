#pragma once

#include <glm/glm_common.h>
#include <string>
#include <Camera.h>
#include <iObject.h>
#include <Components/cMaterial.h>
#include <Physics/octree.h>
#include <FBO/cFBO.h>
#include <cGameObject.h>

class cGameObject;
class cAABB;

class RenderingEngine
{
private:

	RenderingEngine();

	int height;
	int width;


	bool bloom_enabled;
	bool DOF_enabled;
	bool volumetric_enabled;

	cGameObject quad;

public:

	mat4 projection;
	mat4 view;

	cGameObject skyBox;
	cTexture noise;
	vec2 screenPos;
	std::string skyboxName;
	int pass_id;

	~RenderingEngine();

	static RenderingEngine& Instance()
	{
		static RenderingEngine instance;
		return instance;
	}

	void Reset();
	void StencilInit();
	void StencilBegin();
	void StencilEnd();

	void SetUpTextureBindings(cMaterial& material);
	void SetUpTextureBindingsForObject(cGameObject* pCurrentObject);
	void Render(iObject& object);
	void Render(cMaterial& material);


	// Old methods
	void DrawObject(cGameObject* objPtr, mat4 const& v, mat4 const& p);
	void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p);
	void RenderGO(cGameObject* object, float width, float height, mat4& p, mat4& v, int& lastShader);
	void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, mat4 p, mat4 v, float dt);
	void RenderSkybox(float width, float height, mat4 p, mat4 v, float dt);
	void RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO);
	void RenderQuadToScreen(cFBO& previousFBO);

};