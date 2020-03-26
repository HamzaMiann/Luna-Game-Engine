#pragma once

#include <glm/glm_common.h>
#include <string>
#include <Camera.h>
#include <iObject.h>
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
	bool clouds_enabled;
	bool clouds_shadows_enabled;
	bool vignette_enabled;
	bool lens_dirt_enabled;

	cGameObject quad;

public:

	mat4 projection;
	mat4 view;

	cGameObject skyBox;
	cTexture noise;
	vec2 screenPos;
	std::string skyboxName;
	int pass_id;

	void Init();
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

	void SetUpTextureBindingsForObject(cGameObject& object);
	void Render(iObject& object);


	// Old methods
	void DrawObject(cGameObject& object, mat4 const& v, mat4 const& p, Shader* s = nullptr);
	void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p);
	void RenderGO(cGameObject& object, float width, float height, mat4& p, mat4& v, int& lastShader);
	void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, mat4 p, mat4 v, float dt);
	void RenderShadowmapToFBO(cSimpleFBO* fbo, float width, float height);
	void RenderSkybox(float width, float height, mat4 p, mat4 v, float dt);
	void RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO);
	void RenderQuadToScreen(cFBO& previousFBO);

};