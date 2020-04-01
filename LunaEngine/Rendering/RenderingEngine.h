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

	cGameObject quad;
	mat4 depthProjectionMatrix = glm::ortho<float>(-200.f, 200.f, -100.f, 100.f, -1000.f, 1000.f);
	vec3 shadowLightPosition = vec3(0.f, 600.f, 500.f);

	std::map<std::string, bool> boolSettings;
	std::map<std::string, float> floatSettings;

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

	inline void SetProperty(std::string name, bool value) { boolSettings[name] = value; }
	inline void SetProperty(std::string name, float value) { floatSettings[name] = value; }

	bool GetBoolProperty(std::string name);
	float GetFloatProperty(std::string name);

	void Reset();
	void StencilInit();
	void StencilBegin();
	void StencilEnd();

	void SetUpTextureBindingsForObject(cGameObject& object);
	void Render(iObject& object);


	// Old methods
	void DrawObject(cGameObject& object, mat4 const& v, mat4 const& p, Shader* s = nullptr);
	void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p);
	void RenderGO(cGameObject& object, float width, float height, const mat4& p, const mat4& v, int& lastShader, bool shadow = false);
	void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, mat4 p, mat4 v, float dt, bool shadow = false);
	void RenderShadowmapToFBO(cSimpleFBO* fbo, float width, float height);
	void RenderSkybox(float width, float height, mat4 p, mat4 v, float dt);
	void RenderLightingToFBO(cFBO& fbo, cFBO& previousFBO, unsigned int shadowTextureID = 0);
	void RenderPostProcessingToScreen(cFBO& previousFBO, unsigned int shadowTextureID = 0);

};