#pragma once

#include <glm/glm_common.h>
#include <string>
#include <Camera.h>
#include <iObject.h>
#include <Physics/octree.h>
#include <FBO/cFBO.h>
#include <cGameObject.h>
#include <interfaces/physics/iPhysicsDebugRenderer.h>
#include <functional>

class cGameObject;
class cAABB;

class RenderingEngine : public nPhysics::iPhysicsDebugRenderer
{
private:

	RenderingEngine();
	void LoadSettingsFromFile();

	int height;
	int width;

	cGameObject quad;
	mat4 depthProjectionMatrix;
	vec3 shadowLightPosition;

	std::map<std::string, bool> boolSettings;
	std::map<std::string, float> floatSettings;

	std::vector<std::function<void(Shader*)>> debugRenderQueue;

	std::vector<cGameObject*> tombs;

public:

	float mDt = 0.f;

	mat4 projection;
	mat4 view;

	cGameObject skyBox;
	cTexture noise;
	vec2 screenPos;
	std::string skyboxName;
	int pass_id;

	cTexture UITexture;

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

	inline void SetWidth(float width) { this->width = width; }
	inline void SetHeight(float height) { this->height = height; }

	void Reset();
	void StencilInit();
	void StencilBegin();
	void StencilEnd();

	void SetUpTextureBindingsForObject(cGameObject& object);
	void Render(iObject& object);


	// Old methods
	void DrawObject(cGameObject& object, Shader* s = nullptr);
	void DrawInstancedObject(cGameObject& object, const std::vector<mat4>& matModels);
	void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p);
	void RenderGO(cGameObject& object, float width, float height, int& lastShader, bool shadow = false);
	void RenderObjectsToFBO(cSimpleFBO* fbo, bool shadow = false);
	void RenderTombsToFBO(cSimpleFBO* fbo);
	void RenderObjectArray(std::vector<cGameObject*> objects);
	void RenderShadowmapToFBO(cSimpleFBO* fbo, float width, float height);
	void RenderSkybox(mat4 p, mat4 v);
	void RenderLightingToFBO(cFBO& fbo, cFBO& previousFBO, unsigned int shadowTextureID = 0);
	void RenderPostProcessingToScreen(cFBO& previousFBO, unsigned int shadowTextureID, unsigned int graveTexture);


	// Inherited via iPhysicsDebugRenderer
	virtual void DrawSphere(const vec3& center, float radius, const vec3& colour) override;

	virtual void DrawSphere(const mat4& transform, float radius, const vec3& colour) override;

	virtual void DrawCube(const vec3& center, const vec3& scale, const vec3& colour) override;

	virtual void DrawLine(const vec3& from, const vec3& to, const vec3& colour) override;

	virtual void DrawTriangle(const vec3& a, const vec3& b, const vec3& c, const vec3& colour) override;

	void DrawDebugObjects();

};