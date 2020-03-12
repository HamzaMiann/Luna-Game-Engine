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
#include <thread>
#include <safe_promise.h>
#include <threading.h>
#include <iostream>

cTexture worleyNoise;
cTexture worleyNoise2;
cWorleyTexture* worleyTexture;
float cloudDensityFactor = 1.f;
float cloudDensityCutoff = 0.1f;
float cloudLightScattering = 2.f;

std::promise<cWorleyTexture*> promise;
std::future<cWorleyTexture*> future;

//safe_promise<cWorleyTexture*> promise;

RenderingEngine::RenderingEngine()
{
	width = 600;
	height = 800;
	pass_id = 1;

	bloom_enabled = true;
	DOF_enabled = true;
	volumetric_enabled = true;

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

	skyboxName = "city";

	screenPos = vec2(0.f, 0.f);
	noise.SetTexture("noise.jpg");

	worleyTexture = cWorleyTexture::Generate(32u, 4u, 15u, 30u);
	size_t width, height;
	unsigned char* data;
	data = worleyTexture->GetDataRGB(width, height);
	cBasicTextureManager::Instance()->Create3DTexture("worley", true, data, width, height, width);
	worleyNoise.SetTexture("worley");
	worleyNoise2.SetTexture("worley");
	delete worleyTexture; worleyTexture = 0;
	future = promise.get_future();

	new std::thread(cWorleyTexture::GenerateAsyncPromise, &promise, 64u, 4u, 15u, 30u);
	
	//worleyTexture = cWorleyTexture::Generate(32u, 4u, 15u, 30u);

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

}

RenderingEngine::~RenderingEngine() {}

void RenderingEngine::Reset()
{
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
		cloudDensityFactor += 0.1f;
	}
	if (Input::GetKey(GLFW_KEY_LEFT_BRACKET))
	{
		cloudDensityFactor -= 0.1f;
		if (cloudDensityFactor < 0.f)
			cloudDensityFactor = 0.f;
	}

	if (Input::GetKey(GLFW_KEY_EQUAL))
	{
		cloudDensityCutoff += 0.01f;
	}
	if (Input::GetKey(GLFW_KEY_MINUS))
	{
		cloudDensityCutoff -= 0.01f;
		if (cloudDensityCutoff < 0.f)
			cloudDensityCutoff = 0.f;
	}

	if (Input::GetKey(GLFW_KEY_UP))
	{
		cloudLightScattering += 0.1f;
	}
	if (Input::GetKey(GLFW_KEY_DOWN))
	{
		cloudLightScattering -= 0.1f;
		if (cloudLightScattering < 0.f)
			cloudLightScattering = 0.f;
	}

	if (!worleyTexture)
	{
		if (future.wait_for(std::chrono::duration<float>(0.f)) == std::future_status::ready)
		{
			worleyTexture = future.get();
			size_t width, height;
			unsigned char* data;
			data = worleyTexture->GetDataRGB(width, height);
			cBasicTextureManager::Instance()->Create3DTexture("worley", true, data, width, height, width);
			worleyNoise.SetTexture("worley");
			worleyNoise2.SetTexture("worley");
		}
	}
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

void RenderingEngine::SetUpTextureBindings(cMaterial& material)
{
	if (pass_id != 1) return;

	Shader& shader = *material.shader;
	cTexture* textures = material.texture;

	shader.SetTexture(textures[0].GetID(), "textSamp00", 0);
	shader.SetTexture(textures[1].GetID(), "textSamp01", 1);
	shader.SetTexture(textures[2].GetID(), "textSamp02", 2);
	shader.SetTexture(textures[3].GetID(), "textSamp03", 3);

	shader.SetVec4("tex_0_3_ratio",
		vec4(
			textures[0].GetBlend(),
			textures[1].GetBlend(),
			textures[2].GetBlend(),
			textures[3].GetBlend()
		)
	);

	return;
}

void RenderingEngine::Render(iObject& object)
{
	cMaterial* material = object.GetComponent<cMaterial>();
	if (material != nullptr)
	{
		Render(*material);
	}
}

void RenderingEngine::Render(cMaterial& material)
{
	Shader& shader = *material.shader;
	sTransform& transform = material.transform;
	Camera& camera = *Camera::main_camera;
	cBasicTextureManager* textureManager = cBasicTextureManager::Instance();
	GLint shaderProgID = shader.GetID();


	bool isScope = material.layer == "scope";
	shader.SetBool("isScope", isScope);

	GLint bIsSkyBox_UL = shader["isSkybox"];
	GLint isReflection = shader["isReflection"];
	GLint isRefraction = shader["isRefraction"];

	// Set Skybox
	GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
	glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0
	glUniform1i(shader["skyBox"], 26);

	if (material.isSkybox)
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		//glCullFace(GL_FRONT_AND_BACK);
		glUniform1i(shader["isReflection"], (int)GL_FALSE);

		glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);
		glUniform1i(isReflection, (int)GL_FALSE);
		glUniform1i(isRefraction, (int)GL_FALSE);

	}
	else
	{
		if (material.layer == "reflect")
		{
			glCullFace(GL_BACK);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			glUniform1i(isReflection, (int)GL_TRUE);
			glUniform1i(isRefraction, (int)GL_FALSE);

		}
		else if (material.layer == "refract")
		{
			glCullFace(GL_BACK);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			glUniform1i(isReflection, (int)GL_FALSE);
			glUniform1i(isRefraction, (int)GL_TRUE);

		}
		else
		{
			glUniform1i(isReflection, (int)GL_FALSE);
			glUniform1i(isRefraction, (int)GL_FALSE);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			// Don't draw back facing triangles (default)
			glCullFace(GL_BACK);
			SetUpTextureBindings(material);
		}
	}

	mat4 m(1.f);


	// ******* TRANSLATION TRANSFORM *********
	mat4 matTrans
		= glm::translate(mat4(1.0f),
			vec3(
				transform.pos.x,
				transform.pos.y,
				transform.pos.z)
		);
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********


	// ******* ROTATION TRANSFORM *********
	m = m * mat4(transform.rotation);
	// ******* ROTATION TRANSFORM *********

	if (pass_id != 1) m = mat4(1.f);

	GLint matModelIT_UL = shader["matModelInverTrans"];
	mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	// ******* SCALE TRANSFORM *********
	/*mat4 scale = glm::scale(mat4(1.0f),
								 vec3(objPtr->scale,
										   objPtr->scale,
										   objPtr->scale));
	m = m * scale;*/
	// ******* SCALE TRANSFORM *********




	GLint matModel_UL = shader["matModel"];
	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(transform.ModelMatrix()));




	glUniform4f(shader["eyeLocation"],
		camera.Eye.x,
		camera.Eye.y,
		camera.Eye.z,
		1.0f
	);

	/*glUniform4f(shader["diffuseColour"],
		objPtr->colour.x,
		objPtr->colour.y,
		objPtr->colour.z,
		objPtr->colour.w
	);*/

	glUniform4f(shader["specularColour"],
		material.specColour.x,
		material.specColour.y,
		material.specColour.z,
		material.specColour.w
	);

	glUniform1i(shader["isUniform"], material.isUniformColour);


	if (pass_id != 1)
	{
		cLightManager::Instance()->Set_Light_Data(shader);
	}

	if (material.isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	sModelDrawInfo& drawInfo = *material.drawInfo;
	glBindVertexArray(drawInfo.VAO_ID);
	glDrawElements(
		GL_TRIANGLES,
		drawInfo.numberOfIndices,
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0);
}



// OLD CALLS


void RenderingEngine::SetUpTextureBindingsForObject(cGameObject& object)
{
	if (pass_id != 1) return;

	Shader& shader = *object.shader;
	cTexture* textures = object.texture;

	shader.SetTexture(textures[0].GetID(), "textSamp00", 0);
	shader.SetTexture(textures[1].GetID(), "textSamp01", 1);
	shader.SetTexture(textures[2].GetID(), "textSamp02", 2);
	shader.SetTexture(textures[3].GetID(), "textSamp03", 3);

	shader.SetVec4("tex_0_3_ratio",
		vec4(
			textures[0].GetBlend(),
			textures[1].GetBlend(),
			textures[2].GetBlend(),
			textures[3].GetBlend()
		)
	);

	return;
}


void RenderingEngine::DrawObject(cGameObject& object, mat4 const& v, mat4 const& p)
{
	Shader& shader = *object.shader;
	Camera& camera = *Camera::main_camera;

	/*

	MISC EFFECTS

	*/
	bool isUnique = object.tag == "ground";
	shader.SetBool("isUnique", (float)isUnique);


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

	shader.SetVec4("diffuseColour",
		vec4(
			object.colour.x,
			object.colour.y,
			object.colour.z,
			object.colour.w
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

	sModelDrawInfo drawInfo;
	if (cVAOManager::Instance().FindDrawInfoByModelName(object.meshName, drawInfo))
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

void RenderingEngine::DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p)
{
	if (node == nullptr) return;

	if (!node->AABB->contains(obj->transform.pos)) return;


	if (!node->has_nodes)
	{
		glUniform1i((*obj->shader)["isWater"], (int)GL_FALSE);

		objPtr->transform.pos = (node->AABB->min + (node->AABB->min + node->AABB->length)) / 2.f;
		objPtr->transform.scale = vec3(node->AABB->length / 2.f);

		DrawObject(*objPtr, v, p);
	}

	for (int i = 0; i < 8; ++i)
		DrawOctree(obj, node->nodes[i], objPtr, v, p);

}

void RenderingEngine::RenderGO(cGameObject& object, float width, float height, mat4& p, mat4& v, int& lastShader)
{

	for (cGameObject*& child : object.children)
	{
		if (child)
			this->RenderGO(*child, width, height, p, v, lastShader);
	}

	if (!object.shader) return;

	Shader& shader = *object.shader;

	//cMaterial* material = object->GetComponent<cMaterial>();
	//if (material != nullptr)
	//{
	//	// TODO
	//}


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

		shader.SetMat4("matProj", p);
		shader.SetMat4("matView", v);
	}

	this->DrawObject(object, v, p);
}

void RenderingEngine::RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, mat4 p, mat4 v, float dt)
{
	// Draw to the frame buffer
	fbo->use();

	// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);      // Enable blend or "alpha" transparency
	//glDisable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//float ratio = width / height;


	int lastShader = -1;

	auto& objects = cEntityManager::Instance().GetEntities();

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

		//objPtr->cmd_group->Update(dt);
		//objPtr->brain->Update(dt);

		if (object.tag == "portal" || object.tag == "portal2") continue;

		this->RenderGO(object, width, height, p, v, lastShader);


	}//for (int index...
	// **************************************************

}

void RenderingEngine::RenderSkybox(float width, float height, mat4 p, mat4 v, float dt)
{
	// RENDER SKYBOX
	cGameObject* objPtr = &skyBox;
	Shader& shader = *objPtr->shader;

	cMaterial* material = objPtr->GetComponent<cMaterial>();
	if (material != nullptr)
	{
		// TODO
	}

	shader.Use();

	shader.SetFloat("iTime", (float)glfwGetTime());
	shader.SetVec2("iResolution", vec2(width, height));
	shader.SetBool("isWater", GL_FALSE);

	shader.SetMat4("matProj", p);
	shader.SetMat4("matView", v);

	this->DrawObject(*objPtr, v, p);
}

void RenderingEngine::RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO)
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

	shader.SetTexture3D(worleyNoise, "worleyTexture", 6);					// WORLEY NOISE TEXTURE
	shader.SetFloat("cloudDensityFactor", cloudDensityFactor);
	shader.SetFloat("cloudDensityCutoff", cloudDensityCutoff);
	shader.SetFloat("cloudLightScattering", cloudLightScattering);

	// 4. Draw a single object (a triangle or quad)
	shader.SetBool("isFinalPass", GL_FALSE);
	shader.SetVec2("iResolution", vec2(width, height));

	mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	pass_id = 2;

	shader.SetMat4("matProj", p);

	this->DrawObject(quad, mat4(1.f), p);
}

void RenderingEngine::RenderQuadToScreen(cFBO& previousFBO)
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
	//shader.SetTexture(noise, "textSamp00", 0);	// LIT SCENE TEXTURE
	shader.SetTexture(previousFBO.positionTexture_ID, "textSamp01", 1);	// POSITION TEXTURE
	shader.SetTexture(previousFBO.normalTexture_ID, "textSamp02", 2);	// LIGHTING DEPTH BUFFER TEXTURE
	shader.SetTexture(previousFBO.bloomTexture_ID, "textSamp03", 3);	// BLOOM CUTOFF TEXTURE
	shader.SetTexture(noise, "textSamp04", 4);							// NOISE TEXTURE
	shader.SetTexture(previousFBO.unlitTexture_ID, "textSamp05", 5);	// REFLECTIVE SURFACES TEXTURE
	//shader.SetTexture(worleyNoise, "worleyTexture", 6);					// WORLEY NOISE TEXTURE
	shader.SetTexture3D(worleyNoise, "worleyTexture", 6);					// WORLEY NOISE TEXTURE
	shader.SetFloat("cloudDensityFactor", cloudDensityFactor);
	shader.SetFloat("cloudDensityCutoff", cloudDensityCutoff);
	shader.SetFloat("cloudLightScattering", cloudLightScattering);

	shader.SetBool("isFinalPass", GL_TRUE);

	mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	shader.SetMat4("matProj", p);

	if (bloom_enabled)	shader.SetBool("bloomEnabled", GL_TRUE);
	else				shader.SetBool("bloomEnabled", GL_FALSE);

	if (DOF_enabled)	shader.SetBool("DOFEnabled", GL_TRUE);
	else				shader.SetBool("DOFEnabled", GL_FALSE);

	if (volumetric_enabled)	shader.SetBool("volumetricEnabled", GL_TRUE);
	else					shader.SetBool("volumetricEnabled", GL_FALSE);


	this->DrawObject(quad, mat4(1.f), p);
}