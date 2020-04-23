#include "RenderingEngine.h"
#include <glm/gtc/type_ptr.hpp>
#include <_GL/GLCommon.h>
#include <Shader/cShaderManager.h>
#include <Texture/cBasicTextureManager.h>
#include <Lighting/cLightManager.h>
#include <Mesh/cVAOManager.h>
#include <cGameObject.h>
#include <Components/cAnimationController.h>
#include <EntityManager/cEntityManager.h>
#include <Texture/cWorleyTexture.h>
#include <InputManager.h>
#include <iostream>
#include <interfaces/physics/iClothComponent.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <fstream>

cTexture worleyNoise;
cTexture perlinNoise;
cTexture lens;
cTexture blendMap;


#define DEBUG_SETTING true

RenderingEngine::RenderingEngine()
{
	SetProperty("bloomEnabled", DEBUG_SETTING);
	SetProperty("DOFEnabled", DEBUG_SETTING);
	SetProperty("volumetricEnabled", DEBUG_SETTING);
	SetProperty("cloudsEnabled", DEBUG_SETTING);
	SetProperty("cloudShadowsEnabled", DEBUG_SETTING);
	SetProperty("vignetteEnabled", DEBUG_SETTING);
	SetProperty("lensDirtEnabled", DEBUG_SETTING);
	SetProperty("shadowsEnabled", DEBUG_SETTING);

	LoadSettingsFromFile();

	SetProperty("cloudDensityFactor", 1.f);
	SetProperty("cloudDensityCutoff", 0.5f);
	SetProperty("cloudLightScattering", 2.f);
	SetProperty("bloomScale", 0.5f);
	SetProperty("mixValue", 1.f);

	SetProperty("switchColour", true);

	depthProjectionMatrix = glm::ortho<float>(-200.f, 200.f, -100.f, 100.f, -1.f, 1000.f);
	shadowLightPosition = vec3(0.f, 200.f, 500.f);
}

void RenderingEngine::LoadSettingsFromFile()
{
	std::ifstream settingsFile("assets/settings.txt");
	if (settingsFile.is_open())
	{
		std::string property;
		while (settingsFile >> property)
		{
			std::string value;
			settingsFile >> value;
			SetProperty(property, (value == "true") ? true : false);
		}
		settingsFile.close();
	}
}

void RenderingEngine::Init()
{
	width = 600;
	height = 800;
	pass_id = 1;

	float ratio = width / (float)height;

	projection = glm::perspective(0.6f,			// FOV
		ratio,		// Aspect ratio
		0.1f,			// Near clipping plane
		1000.f);	// Far clipping plane

	view = mat4(1.f);

	quad.meshName = "screen_quad";
	quad.shader = Shader::FromName("post");

	skyBox.transform.pos = vec3(0.f);
	skyBox.meshName = "sphere";
	skyBox.shader = Shader::FromName("basic");
	skyBox.tag = "skybox";
	skyBox.transform.scale = vec3(900.0f);

	skyboxName = "default";

	screenPos = vec2(0.f, 0.f);
	noise.SetTexture("noise.jpg");

	//lens.SetTexture("lens_dust.jpg");
	//lens.SetTexture("soap-008.png");
	//lens.SetTexture("dirt.jpg");
	lens.SetTexture("dust_particles_on_lens_by_kerast_d7mj0cr-fullview.jpg");

	blendMap.SetTexture("WATER_BUMP.png", 1.f);


	if (GetBoolProperty("cloudsEnabled"))
	{
		sTextureData tex;
		cBasicTextureManager::Instance()->LoadWorleyFromFile("assets/textures/clouds256.matrix", tex);
		cBasicTextureManager::Instance()->Create3DTexture("worley", true, &tex.data[0], tex.width, tex.height, tex.width);
		worleyNoise.SetTexture("worley");
	}

	perlinNoise.SetTexture("perlin.png");

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing
}

RenderingEngine::~RenderingEngine() {}


bool RenderingEngine::GetBoolProperty(std::string name)
{
	auto it = boolSettings.find(name);
	if (it == boolSettings.end()) return false;
	return it->second;
}

float RenderingEngine::GetFloatProperty(std::string name)
{
	auto it = floatSettings.find(name);
	if (it == floatSettings.end()) return 0.f;
	return it->second;
}

void RenderingEngine::Reset()
{
	//pass_id = 1;

	tombs.clear();

	UITexture.SetTexture(0);

	view = glm::lookAt(
		Camera::main_camera->Eye,
		Camera::main_camera->Target,
		Camera::main_camera->Up
	);

	// RESET
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	// Enable writing to the colour buffer
	glDepthMask(GL_TRUE);								// Enable writing to the depth buffer
	glEnable(GL_DEPTH_TEST);							// Enable depth testing
	glDisable(GL_STENCIL_TEST);							// Disable stencil test

	if (Input::GetKey(GLFW_KEY_RIGHT_BRACKET))
	{
		floatSettings["cloudDensityFactor"] += 0.1f;
	}
	if (Input::GetKey(GLFW_KEY_LEFT_BRACKET))
	{
		floatSettings["cloudDensityFactor"] -= 0.1f;
		if (floatSettings["cloudDensityFactor"] < 0.f)
			floatSettings["cloudDensityFactor"] = 0.f;
	}

	if (Input::GetKey(GLFW_KEY_EQUAL))
	{
		floatSettings["cloudDensityCutoff"] += 0.01f;
	}
	if (Input::GetKey(GLFW_KEY_MINUS))
	{
		floatSettings["cloudDensityCutoff"] -= 0.01f;
		if (floatSettings["cloudDensityCutoff"] < 0.f)
			floatSettings["cloudDensityCutoff"] = 0.f;
	}

	if (Input::KeyDown(GLFW_KEY_0))
	{
		boolSettings["switchColour"] = !boolSettings["switchColour"];
	}

	/*if (Input::GetKey(GLFW_KEY_UP))
	{
		cloudLightScattering += 0.1f;
	}
	if (Input::GetKey(GLFW_KEY_DOWN))
	{
		cloudLightScattering -= 0.1f;
		if (cloudLightScattering < 0.f)
			cloudLightScattering = 0.f;
	}*/
}

void RenderingEngine::StencilInit()
{
	glClearStencil(47);
	glClear(GL_STENCIL_BUFFER_BIT);

	glEnable(GL_STENCIL_TEST);

	glStencilOp(GL_KEEP,		// Stencil fails KEEP the original value (47)
		GL_KEEP,		// Depth fails KEEP the original value
		GL_REPLACE);	// Stencil AND depth PASSES, REPLACE with 133

	glStencilFunc(GL_ALWAYS,	// If is succeed, ALWAYS do this
		133,			// Replace with this
		0xFF);		// Mask of 1111,1111 (no mask)

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
}

void RenderingEngine::StencilBegin()
{
	glDepthMask(GL_TRUE);

	glClear(GL_DEPTH_BUFFER_BIT);


	// Change the stencil test
	glStencilOp(GL_KEEP,		// Stencil fails KEEP the original value (47)
		GL_KEEP,		// (stencil passes) Depth fails KEEP the original value
		GL_KEEP);		// Stencil AND depth PASSES, Keep 133
	glStencilFunc(GL_EQUAL,		// Test if equal
		133,			//
		0xFF);


	// Draw the "inside the room" scene...
	// (only visible where the "door" model drew 133 to the stencil buffer
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void RenderingEngine::StencilEnd()
{
	glDisable(GL_STENCIL_TEST);
}


void RenderingEngine::SetUpTextureBindingsForObject(cGameObject& object)
{
	if (pass_id != 1) return;

	Shader& shader = *object.shader;
	cTexture* textures = object.texture;

	shader.SetTexture(textures[0].GetID(), "textSamp00", 0);
	shader.SetTexture(textures[1].GetID(), "textSamp01", 1);
	shader.SetTexture(textures[2].GetID(), "textSamp02", 2);
	shader.SetTexture(textures[3].GetID(), "textSamp03", 3);

	
	if (object.shouldBlend)
	{
		shader.SetBool("useBlendMap", true);
		shader.SetTexture(object.blendMap, "blendMap", 4);
		shader.SetFloat("blendTiling", object.blendMap.GetTiling());
	}
	else
	{
		shader.SetBool("useBlendMap", false);
	}

	shader.SetVec4("tex_tiling",
		vec4(
			textures[0].GetTiling(),
			textures[1].GetTiling(),
			textures[2].GetTiling(),
			textures[3].GetTiling()
		)
	);

	shader.SetVec4("tex_0_3_ratio",
		vec4(
			textures[0].GetBlend(),
			textures[1].GetBlend(),
			textures[2].GetBlend(),
			textures[3].GetBlend()
		)
	);

	shader.SetBool("isPBR", false);

	return;
}


void RenderingEngine::DrawObject(cGameObject& object, Shader* s)
{
	// if the model does not exist, don't do anything
	sModelDrawInfo drawInfo;
	if (!cVAOManager::Instance().FindDrawInfoByModelName(object.meshName, drawInfo)) return;
	



	Shader& shader = *object.shader;
	if (s) {
		shader = *s;
	}
	Camera& camera = *Camera::main_camera;


	/*

	ANIMATION

	*/
	cAnimationController* animator = object.GetComponent<cAnimationController>();

	if (animator)
	{
		auto& transforms = animator->GetTransformations();
		shader.SetBool("isSkinnedMesh", (float)GL_TRUE);
		shader.SetMat4Array("matBonesArray", transforms);
	}
	else shader.SetBool("isSkinnedMesh", (float)GL_FALSE);


	/*

	REFLECTION AND REFRACTION

	*/
	shader.SetBool("reflectivity", object.reflectivity);
	shader.SetBool("refractivity", object.refractivity);


	/*

	SKYBOX

	*/
	shader.SetCubemap("skyBox", cBasicTextureManager::Instance()->getTextureIDFromName(skyboxName));
	if (object.tag != "skybox")
	{
		// Don't draw back facing triangles (default)
		glCullFace(GL_BACK);
		shader.SetBool("isSkybox", GL_FALSE);
		SetUpTextureBindingsForObject(object);
	}
	else
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		shader.SetBool("isSkybox", GL_TRUE);
	}

	mat4 m(1.f);

	if (object.parent)
	{
		m *= object.transform.TranslationMatrix(object.parent->transform);
		m *= object.transform.RotationMatrix(object.parent->transform);
	}
	else
	{
		m *= object.transform.TranslationMatrix();
		m *= object.transform.RotationMatrix();
	}



	mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(shader["matModelInverTrans"], 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	if (object.parent)
		m *= object.transform.ScaleMatrix(object.parent->transform);
	else
		m *= object.transform.ScaleMatrix();

	if (pass_id != 1) m = mat4(1.f);

	shader.SetMat4("matModel", m);

	shader.SetVec4("eyeLocation",
		vec4(
			camera.Eye.x,
			camera.Eye.y,
			camera.Eye.z,
			1.0f
		)
	);

	shader.SetVec4("eyeTarget",
		vec4(
			camera.Target.x,
			camera.Target.y,
			camera.Target.z,
			1.0f
		)
	);

	shader.SetVec4("specularColour",
		vec4(
			object.specColour.x,
			object.specColour.y,
			object.specColour.z,
			object.specIntensity
		)
	);

	shader.SetBool("isUniform", object.uniformColour);


	if (pass_id != 1)
	{
		cLightManager::Instance()->Set_Light_Data(shader);
		shader.SetVec2("lightPositionOnScreen", screenPos);
	}


	// This will change the fill mode to something 
	//  GL_FILL is solid 
	//  GL_LINE is "wireframe"
	//glPointSize(15.0f);

	if (object.isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	
	glBindVertexArray(drawInfo.VAO_ID);
	glDrawElements(
		GL_TRIANGLES,
		drawInfo.numberOfIndices,
		GL_UNSIGNED_INT,
		0
	);
	glBindVertexArray(0);
}


void RenderingEngine::DrawInstancedObject(cGameObject& object, const std::vector<mat4>& matModels)
{
	// if the model does not exist, don't do anything
	sModelDrawInfo drawInfo;
	if (!cVAOManager::Instance().FindDrawInfoByModelName(object.meshName, drawInfo)) return;

	Shader& shader = *object.shader;
	Camera& camera = *Camera::main_camera;

	shader.Use();

	// set time
	float time = glfwGetTime();

	shader.SetFloat("iTime", (float)glfwGetTime());
	shader.SetVec2("iResolution", vec2(width, height));

	shader.SetMat4("matProj", projection);
	shader.SetMat4("matView", view);

	shader.SetBool("isShadowMap", false);


	/*

	REFLECTION AND REFRACTION

	*/
	shader.SetBool("reflectivity", object.reflectivity);
	shader.SetBool("refractivity", object.refractivity);


	// TEXTURES
	// Don't draw back facing triangles (default)
	glCullFace(GL_BACK);
	shader.SetBool("isSkybox", GL_FALSE);
	SetUpTextureBindingsForObject(object);

	shader.SetVec4("eyeLocation",
		vec4(
			camera.Eye.x,
			camera.Eye.y,
			camera.Eye.z,
			1.0f
			)
		);

	shader.SetVec4("eyeTarget",
		vec4(
			camera.Target.x,
			camera.Target.y,
			camera.Target.z,
			1.0f
			)
		);

	shader.SetVec4("specularColour",
		vec4(
			object.specColour.x,
			object.specColour.y,
			object.specColour.z,
			object.specIntensity
			)
		);

	shader.SetBool("isUniform", object.uniformColour);


	if (object.isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glBindVertexArray(drawInfo.VAO_ID);
	glDrawElementsInstanced(GL_TRIANGLES, drawInfo.numberOfIndices, GL_UNSIGNED_INT, 0, matModels.size());
	glBindVertexArray(0);
}


void RenderingEngine::DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p)
{
	if (node == nullptr) return;

	if (!node->AABB->contains(obj->transform.pos)) return;


	if (!node->has_nodes)
	{
		glUniform1i((*obj->shader)["isWater"], (int)GL_FALSE);

		objPtr->transform.pos = (node->AABB->min + (node->AABB->min + node->AABB->length)) / 2.f;
		objPtr->transform.scale = vec3(node->AABB->length / 2.f);

		DrawObject(*objPtr);
	}

	for (int i = 0; i < 8; ++i)
		DrawOctree(obj, node->nodes[i], objPtr, v, p);

}

void RenderingEngine::RenderGO(cGameObject& object, float width, float height, int& lastShader, bool shadow)
{
	for (iObject* child : object.Children)
	{
		if (child)
			this->RenderGO(*reinterpret_cast<cGameObject*>(child), width, height, lastShader);
	}

	if (!object.shader) return;

	Shader& shader = *object.shader;

	// Only switch shaders if needed
	if (lastShader != shader.GetID())
	{
		shader.Use();
		lastShader = shader.GetID();

		// set time
		float time = glfwGetTime();

		shader.SetFloat("iTime", (float)glfwGetTime());
		shader.SetVec2("iResolution", vec2(width, height));
		shader.SetBool("isWater", GL_FALSE);

		shader.SetMat4("matProj", projection);
		shader.SetMat4("matView", view);

		if (shadow)
		{
			shader.SetBool("isShadowMap", true);
		}
		else
		{
			shader.SetBool("isShadowMap", false);
		}

		shader.SetBool("isTomb", GL_FALSE);

	}

	nPhysics::iClothComponent* cloth = object.GetComponent<nPhysics::iClothComponent>();
	if (cloth)
	{
		for (size_t i = 0; i < cloth->NumNodes(); ++i)
		{
			cloth->GetNodePosition(i, object.transform.pos);
			float radius;
			cloth->GetNodeRadius(i, radius);
			object.transform.Scale(vec3(radius));
			DrawObject(object);
		}

#ifdef _DEBUG
		float t = cLowpassFilter::Instance().delta_time();
		for (size_t i = 0; i < cloth->NumSprings(); ++i)
		{
			std::pair<size_t, size_t> pair;
			cloth->GetSpring(i, pair);
			vec3 first, second;
			cloth->GetNodePosition(pair.first, first);
			cloth->GetNodePosition(pair.second, second);
			cDebugRenderer::Instance().addLine(first, second, vec3(1.f, 0.f, 1.f), t);
		}
#endif
	}
	else
		this->DrawObject(object);
}

void RenderingEngine::RenderObjectsToFBO(cSimpleFBO* fbo, bool shadow)
{
	// Draw to the frame buffer
	fbo->use();
	fbo->clear_all();

	// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_BLEND);      // Enable blend or "alpha" transparency
	glDisable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//float ratio = width / height;


	int lastShader = -1;

	auto& objects = cEntityManager::Instance().GetEntities();

	if (shadow)
	{
		mat4 depthViewMatrix = glm::lookAt(
			shadowLightPosition,
			//vec3(cLightManager::Instance()->Lights[0]->position),
			vec3(0.f),
			vec3(0, 1, 0)
			);
		projection = depthProjectionMatrix;
		view = depthViewMatrix;
	}

	// Loop to draw everything in the scene
	for (int index = 0; index != objects.size(); index++)
	{
#ifdef TRANSPARENCY_SORT
		if (index < scene->vecGameObjects.size() - 1)
		{
			vec3 ObjA = objects[index]->transform.pos;
			vec3 ObjB = objects[index + 1]->transform.pos;

			//			if ( glm::distance( ObjA, ::g_pFlyCamera->eye ) < glm::distance( ObjB, ::g_pFlyCamera->eye ) )
			if (glm::distance2(ObjA, scene->camera.Eye) < glm::distance2(ObjB, scene->camera.Eye))
			{
				// Out of order, so swap the positions...
				cGameObject* pTemp = scene->vecGameObjects[index];
				scene->vecGameObjects[index] = scene->vecGameObjects[index + 1];
				scene->vecGameObjects[index + 1] = pTemp;
			}
		}
#endif

		cGameObject& object = *objects[index];

		if (!shadow && object.meshName == "tomb") tombs.push_back(&object);

		this->RenderGO(object, width, height, lastShader, shadow);


	}//for (int index...
	// **************************************************

}

void RenderingEngine::RenderTombsToFBO(cSimpleFBO* fbo)
{
	// Draw to the frame buffer
	fbo->use();
	fbo->clear_all();

	// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_BLEND);      // Enable blend or "alpha" transparency
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//float ratio = width / height;


	int lastShader = -1;

	auto& objects = tombs;

	// Loop to draw everything in the scene
	for (int index = 0; index != objects.size(); index++)
	{

		cGameObject& object = *objects[index];
		Shader& shader = *object.shader;

		// Only switch shaders if needed
		if (lastShader != shader.GetID())
		{
			shader.Use();
			lastShader = shader.GetID();

			// set time
			float time = glfwGetTime();

			shader.SetFloat("iTime", (float)glfwGetTime());
			shader.SetVec2("iResolution", vec2(width, height));
			shader.SetBool("isWater", GL_FALSE);

			shader.SetMat4("matProj", projection);
			shader.SetMat4("matView", view);

			shader.SetBool("isShadowMap", false);
		}

		shader.SetBool("isTomb", GL_TRUE);

		this->DrawObject(object);


	}//for (int index...
}

void RenderingEngine::RenderObjectArray(std::vector<cGameObject*> objects)
{
	int lastShader = -1;
	for (auto* object : objects)
	{
		RenderGO(*object, 0, 0, lastShader, false);
	}
}

void RenderingEngine::RenderShadowmapToFBO(cSimpleFBO* fbo, float width, float height)
{
	// Draw to the frame buffer
	fbo->use();
	fbo->clear_all();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	auto& objects = cEntityManager::Instance().GetEntities();

	Shader& shader = *Shader::FromName("shadow");
	shader.Use();

	mat4 depthProjectionMatrix = glm::ortho<float>(-100.f, 100.f, -100.f, 100.f, -1000.f, 1000.f);
	mat4 depthViewMatrix = glm::lookAt(
			vec3(cLightManager::Instance()->Lights[0]->position),
			vec3(0.f),
			//Camera::main_camera->Target,
			vec3(0, 1, 0)
		);
	//mat4 depthModelMatrix = mat4(1.0);
	//mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	shader.SetMat4("matProj", depthProjectionMatrix);
	shader.SetMat4("matView", depthViewMatrix);

	// Loop to draw everything in the scene
	for (int index = 0; index != objects.size(); index++)
	{
		DrawObject(*objects[index], &shader);
	}
}

void RenderingEngine::RenderSkybox(mat4 p, mat4 v)
{
	// RENDER SKYBOX
	cGameObject* objPtr = &skyBox;
	Shader& shader = *objPtr->shader;

	shader.Use();

	shader.SetFloat("iTime", (float)glfwGetTime());
	shader.SetVec2("iResolution", vec2(width, height));
	shader.SetBool("isWater", GL_FALSE);

	shader.SetMat4("matProj", p);
	shader.SetMat4("matView", v);

	this->DrawObject(*objPtr);
}

void RenderingEngine::RenderLightingToFBO(cFBO& fbo, cFBO& previousFBO, unsigned int shadowTextureID)
{
	float width = previousFBO.width;
	float height = previousFBO.height;

	// 1. Disable the FBO
	// Draw to the frame buffer
	fbo.use();
	fbo.clear_all();

	// 2. Clear the ACTUAL screen buffer
	/*glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

	// 3. Use the FBO colour texture as the texture on that quad
	Shader& shader = *quad.shader;
	shader.Use();

	shader.SetTexture(previousFBO.colourTexture_ID, "textSamp00", 0);	// ALBEDO TEXTURE
	shader.SetTexture(previousFBO.normalTexture_ID, "textSamp01", 1);	// NORMAL TEXTURE
	shader.SetTexture(previousFBO.positionTexture_ID, "textSamp02", 2);	// POSITION TEXTURE
	shader.SetTexture(previousFBO.bloomTexture_ID, "textSamp03", 3);	// BLOOM TEXTURE (NOT USED ON THIS PASS)
	shader.SetTexture(previousFBO.unlitTexture_ID, "textSamp04", 4);	// TEXTURE INDICATING UNLIT OBJECTS

	shader.SetTexture3D(worleyNoise, "worleyTexture", 6);				// WORLEY NOISE TEXTURE
	shader.SetTexture(perlinNoise, "perlinTexture", 7);					// PERLIN NOISE TEXTURE
	shader.SetTexture(shadowTextureID, "shadowTexture", 8);				// SHADOWMAP TEXTURE

	for (auto prop : boolSettings)
	{
		shader.SetBool(prop.first, prop.second);
	}

	for (auto prop : floatSettings)
	{
		shader.SetFloat(prop.first, prop.second);
	}

	// 4. Draw a single object (a triangle or quad)
	shader.SetBool("isFinalPass", GL_FALSE);
	shader.SetVec2("iResolution", vec2(width, height));


	mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	pass_id = 2;

	{
		mat4 depthViewMatrix = glm::lookAt(
			shadowLightPosition,
			//vec3(cLightManager::Instance()->Lights[0]->position),
			vec3(0.f),
			//Camera::main_camera->Target,
			vec3(0, 1, 0)
			);
		mat4 mvp = depthProjectionMatrix * depthViewMatrix;
		/*glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);*/
		//glm::mat4 depthBiasMVP = mvp;
		//glm::mat4 depthBiasMVP = biasMatrix * mvp;
		shader.SetMat4("shadowMVP", mvp);
	}

	shader.SetMat4("matProj", p);

	this->DrawObject(quad);
}

void RenderingEngine::RenderPostProcessingToScreen(cFBO& previousFBO, unsigned int shadowTextureID, unsigned int graveTexture)
{
	// LAST RENDER PASS

	float width = previousFBO.width;
	float height = previousFBO.height;

	Shader& shader = *quad.shader;

	// 1. Disable the FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. Clear the ACTUAL screen buffer
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();

	shader.SetFloat("iTime", (float)glfwGetTime());

	shader.SetTexture(previousFBO.colourTexture_ID, "textSamp00", 0);	// LIT SCENE TEXTURE
	shader.SetTexture(previousFBO.positionTexture_ID, "textSamp01", 1);	// POSITION TEXTURE
	shader.SetTexture(previousFBO.normalTexture_ID, "textSamp02", 2);	// LIGHTING DEPTH BUFFER TEXTURE
	shader.SetTexture(previousFBO.bloomTexture_ID, "textSamp03", 3);	// BLOOM CUTOFF TEXTURE
	shader.SetTexture(noise, "textSamp04", 4);							// NOISE TEXTURE
	shader.SetTexture(previousFBO.unlitTexture_ID, "textSamp05", 5);	// REFLECTIVE SURFACES TEXTURE
	shader.SetTexture3D(worleyNoise, "worleyTexture", 6);				// WORLEY NOISE TEXTURE

	shader.SetTexture(lens, "lensTexture", 8);							// LENS NOISE TEXTURE
	shader.SetTexture(shadowTextureID, "perlinTexture", 9);				// SHADOWMAP TEXTURE
	shader.SetTexture(UITexture, "UITexture", 10);						// UI TEXTURE
	shader.SetTexture(graveTexture, "GraveTexture", 11);						// UI TEXTURE

	shader.SetBool("isFinalPass", GL_TRUE);

	mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	shader.SetMat4("matProj", p);

	for (auto prop : boolSettings)
	{
		shader.SetBool(prop.first, prop.second);
	}

	for (auto prop : floatSettings)
	{
		shader.SetFloat(prop.first, prop.second);
	}



	this->DrawObject(quad);
}

void RenderingEngine::DrawSphere(const vec3& center, float radius, const vec3& colour)
{
	debugRenderQueue.push_back([this, center, radius, colour](Shader* shader)
		{
			sTransform tform;
			tform.Position(center);
			tform.Scale(vec3(radius));
			
			shader->SetMat4("matModel", tform.ModelMatrix());
			shader->SetVec3("fColour", colour);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			sModelDrawInfo drawInfo;
			if (cVAOManager::Instance().FindDrawInfoByModelName("sphere", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(
					GL_TRIANGLES,
					drawInfo.numberOfIndices,
					GL_UNSIGNED_INT,
					0
					);
				glBindVertexArray(0);
			}
		}
	);
}

void RenderingEngine::DrawSphere(const mat4& transform, float radius, const vec3& colour)
{
	debugRenderQueue.push_back([this, transform, radius, colour](Shader* shader)
		{
			mat4 model = transform * glm::scale(mat4(1.0f), vec3(radius));

			shader->SetMat4("matModel", model);
			shader->SetVec3("fColour", colour);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			sModelDrawInfo drawInfo;
			if (cVAOManager::Instance().FindDrawInfoByModelName("sphere", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(
					GL_TRIANGLES,
					drawInfo.numberOfIndices,
					GL_UNSIGNED_INT,
					0
					);
				glBindVertexArray(0);
			}
		}
	);
}

void RenderingEngine::DrawCube(const vec3& center, const vec3& scale, const vec3& colour)
{
	debugRenderQueue.push_back([this, center, scale, colour](Shader* shader)
		{
			sTransform tform;
			tform.Position(center);
			tform.Scale(scale);
			
			shader->SetMat4("matModel", tform.ModelMatrix());
			shader->SetVec3("fColour", colour);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			sModelDrawInfo drawInfo;
			if (cVAOManager::Instance().FindDrawInfoByModelName("cube", drawInfo))
			{
				glBindVertexArray(drawInfo.VAO_ID);
				glDrawElements(
					GL_TRIANGLES,
					drawInfo.numberOfIndices,
					GL_UNSIGNED_INT,
					0
					);
				glBindVertexArray(0);
			}
		}
	);
}

void RenderingEngine::DrawLine(const vec3& from, const vec3& to, const vec3& colour)
{
	cDebugRenderer::Instance().addLine(from, to, colour, mDt);
}

void RenderingEngine::DrawTriangle(const vec3& a, const vec3& b, const vec3& c, const vec3& colour)
{
	cDebugRenderer::Instance().addTriangle(a, b, c, colour, mDt);
}

void RenderingEngine::DrawDebugObjects()
{
	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	Shader* shader = Shader::FromName("debug");
	shader->Use();
	shader->SetMat4("matProj", projection);
	shader->SetMat4("matView", view);
	for (auto func : debugRenderQueue)
	{
		func(shader);
	}
	debugRenderQueue.clear();
	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing
}
