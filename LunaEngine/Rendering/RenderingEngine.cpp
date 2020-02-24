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


RenderingEngine::RenderingEngine()
{
	width = 600;
	height = 800;
	pass_id = 1;

	float ratio = width / (float)height;

	projection = glm::perspective(0.6f,		// FOV
								  ratio,			// Aspect ratio
								  0.1f,			// Near clipping plane
								  100'000.f);		// Far clipping plane

	view = mat4(1.f);

	quad.meshName = "screen_quad";
	quad.shader = Shader::FromName("post");
	skyBox.transform.pos = vec3(0.f);
	skyBox.meshName = "sphere";
	skyBox.shader = Shader::FromName("basic");
	skyBox.tag = "skybox";
	skyBox.transform.scale = vec3(900.0f);
	screenPos = vec2(0.f, 0.f);
	skyboxName = "sky";
	noise.SetTexture("noise.jpg");
}

RenderingEngine::~RenderingEngine()
{
}

void RenderingEngine::UpdateView()
{
	view = glm::lookAt(
		Camera::main_camera->Eye,
		Camera::main_camera->Target,
		Camera::main_camera->Up
	);
}

void RenderingEngine::SetUpTextureBindings(cMaterial& material)
{
	if (pass_id != 1) return;

	Shader& shader = *material.shader;
	cTexture* textures = material.texture;


	glActiveTexture(GL_TEXTURE0);						// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, textures[0].GetID());	// Texture now assoc with texture unit 0

	glActiveTexture(GL_TEXTURE1);						// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, textures[1].GetID());	// Texture now assoc with texture unit 1

	glActiveTexture(GL_TEXTURE2);						// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, textures[2].GetID());	// Texture now assoc with texture unit 2

	glActiveTexture(GL_TEXTURE3);						// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, textures[3].GetID());	// Texture now assoc with texture unit 3

	glUniform1i(shader["textSamp00"], 0);	// Texture unit 0

	glUniform1i(shader["textSamp01"], 1);	// Texture unit 1

	glUniform1i(shader["textSamp02"], 2);	// Texture unit 2

	glUniform1i(shader["textSamp03"], 3);	// Texture unit 3


	glUniform4f(shader["tex_0_3_ratio"],
		textures[0].GetBlend(),
		textures[1].GetBlend(),
		textures[2].GetBlend(),
		textures[3].GetBlend()
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

	GLint scopeUL = shader["isScope"];
	if (material.layer == "scope")
	{
		glUniform1i(scopeUL, (int)GL_TRUE);
	}
	else
	{
		glUniform1i(scopeUL, (int)GL_FALSE);
	}



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


void RenderingEngine::SetUpTextureBindingsForObject(cGameObject* pCurrentObject)
{
	if (pass_id != 1) return;

	Shader& shader = *pCurrentObject->shader;
	GLint shaderProgID = shader.GetID();

	// Tie the texture to the texture unit
	GLuint texSamp0_UL = pCurrentObject->texture[0].GetID();
	glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp1_UL = pCurrentObject->texture[1].GetID();
	glActiveTexture(GL_TEXTURE1);				// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, texSamp1_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp2_UL = pCurrentObject->texture[2].GetID();
	glActiveTexture(GL_TEXTURE2);				// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, texSamp2_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp3_UL = pCurrentObject->texture[3].GetID();
	glActiveTexture(GL_TEXTURE3);				// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, texSamp3_UL);	// Texture now assoc with texture unit 0

	// Tie the texture units to the samplers in the shader
	GLint textSamp00_UL = shader["textSamp00"];
	glUniform1i(textSamp00_UL, 0);	// Texture unit 0

	GLint textSamp01_UL = shader["textSamp01"];
	glUniform1i(textSamp01_UL, 1);	// Texture unit 1

	GLint textSamp02_UL = shader["textSamp02"];
	glUniform1i(textSamp02_UL, 2);	// Texture unit 2

	GLint textSamp03_UL = shader["textSamp03"];
	glUniform1i(textSamp03_UL, 3);	// Texture unit 3


	GLint tex0_ratio_UL = shader["tex_0_3_ratio"];
	glUniform4f(tex0_ratio_UL,
		pCurrentObject->texture[0].GetBlend(),
		pCurrentObject->texture[1].GetBlend(),
		pCurrentObject->texture[2].GetBlend(),
		pCurrentObject->texture[3].GetBlend()
	);

	return;
}


void RenderingEngine::DrawObject(cGameObject* objPtr, mat4 const& v, mat4 const& p)
{
	Shader& shader = *objPtr->shader;
	GLint shaderProgID = shader.GetID();

	/*

	MISC EFFECTS

	*/
	if (objPtr->tag == "scope") glUniform1i(shader["isScope"], (int)GL_TRUE);
	else glUniform1i(shader["isScope"], (int)GL_FALSE);


	/*

	ANIMATION

	*/
	cAnimationController* animator = objPtr->GetComponent<cAnimationController>();

	if (animator)
	{
		glUniform1f(shader["isSkinnedMesh"], (float)GL_TRUE);
		auto& transforms = animator->GetTransformations();
		GLint matBonesArray_UniLoc = glGetUniformLocation(shaderProgID, "matBonesArray");
		GLint numBonesUsed = (GLint)transforms.size();
		glUniformMatrix4fv(matBonesArray_UniLoc, numBonesUsed,
			GL_FALSE,
			glm::value_ptr(transforms[0]));
	}
	else glUniform1f(shader["isSkinnedMesh"], (float)GL_FALSE);


	/*

	REFLECTION AND REFRACTION

	*/
	glUniform1f(shader["reflectivity"], objPtr->reflectivity);
	glUniform1f(shader["refractivity"], objPtr->refractivity);


	/*

	SKYBOX

	*/
	GLint bIsSkyBox_UL = shader["isSkybox"];
	if (objPtr->tag != "skybox")
	{
		// Don't draw back facing triangles (default)
		glCullFace(GL_BACK);
		glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);

		GLuint skyBoxTextureID = cBasicTextureManager::Instance()->getTextureIDFromName(skyboxName);
		glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		// Tie the texture units to the samplers in the shader
		GLint skyBoxSampler_UL = shader["skyBox"];
		glUniform1i(skyBoxSampler_UL, 26);

		SetUpTextureBindingsForObject(objPtr);
	}
	else
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		//glCullFace(GL_FRONT_AND_BACK);

		glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);

		GLuint skyBoxTextureID = cBasicTextureManager::Instance()->getTextureIDFromName(skyboxName);
		glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		// Tie the texture units to the samplers in the shader
		GLint skyBoxSampler_UL = shader["skyBox"];
		glUniform1i(skyBoxSampler_UL, 26);	// Texture unit 26
	}

	mat4 m(1.f);

	if (objPtr->parent)
	{
		m *= objPtr->transform.TranslationMatrix(objPtr->parent->transform);
		m *= objPtr->transform.RotationMatrix(objPtr->parent->transform);
	}
	else
	{
		m *= objPtr->transform.TranslationMatrix();
		m *= objPtr->transform.RotationMatrix();
	}



	mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(shader["matModelInverTrans"], 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	if (objPtr->parent)
		m *= objPtr->transform.ScaleMatrix(objPtr->parent->transform);
	else
		m *= objPtr->transform.ScaleMatrix();

	if (pass_id != 1) m = mat4(1.f);


	glUniformMatrix4fv(shader["matModel"], 1, GL_FALSE, glm::value_ptr(m));

	Camera& camera = *Camera::main_camera;

	glUniform4f(shader["eyeLocation"],
		camera.Eye.x,
		camera.Eye.y,
		camera.Eye.z,
		1.0f
	);

	glUniform4f(shader["eyeTarget"],
		camera.Target.x,
		camera.Target.y,
		camera.Target.z,
		1.0f
	);


	glUniform4f(shader["diffuseColour"],
		objPtr->colour.x,
		objPtr->colour.y,
		objPtr->colour.z,
		objPtr->colour.w
	);

	glUniform4f(shader["specularColour"],
		objPtr->specColour.x,
		objPtr->specColour.y,
		objPtr->specColour.z,
		objPtr->specIntensity
	);

	glUniform1i(shader["isUniform"], objPtr->uniformColour);


	if (pass_id != 1)
	{
		cLightManager::Instance()->Set_Light_Data(shader);
		glUniform2f(shader["lightPositionOnScreen"], screenPos.x, screenPos.y);
	}


	// This will change the fill mode to something 
	//  GL_FILL is solid 
	//  GL_LINE is "wireframe"
	//glPointSize(15.0f);

	if (objPtr->isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	sModelDrawInfo drawInfo;
	if (cVAOManager::Instance().FindDrawInfoByModelName(objPtr->meshName, drawInfo))
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

		DrawObject(objPtr, v, p);
	}

	for (int i = 0; i < 8; ++i)
		DrawOctree(obj, node->nodes[i], objPtr, v, p);

}

void RenderingEngine::RenderGO(cGameObject* object, float width, float height, mat4& p, mat4& v, int& lastShader)
{

	for (cGameObject*& child : object->children)
	{
		this->RenderGO(child, width, height, p, v, lastShader);
	}

	if (!object->shader) return;

	Shader& shader = *object->shader;
	GLint shaderProgID = shader.GetID();

	//cMaterial* material = object->GetComponent<cMaterial>();
	//if (material != nullptr)
	//{
	//	// TODO
	//}


	// Only switch shaders if needed
	if (lastShader != shaderProgID)
	{
		glUseProgram(shaderProgID);
		lastShader = shaderProgID;

		// set time
		float time = glfwGetTime();

		glUniform1f(shader["iTime"], time);

		// set resolution
		glUniform2f(shader["iResolution"],
			width,
			height);

		glUniform1i(shader["isWater"],
			false);

		glUniformMatrix4fv(shader["matView"], 1, GL_FALSE, glm::value_ptr(v));
		glUniformMatrix4fv(shader["matProj"], 1, GL_FALSE, glm::value_ptr(p));
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

		cGameObject* objPtr = objects[index];

		//objPtr->cmd_group->Update(dt);
		//objPtr->brain->Update(dt);

		if (objPtr->tag == "portal" || objPtr->tag == "portal2") continue;

		this->RenderGO(objPtr, width, height, p, v, lastShader);


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

	//objPtr->cmd_group->Update(dt);
	//objPtr->brain->Update(dt);

	GLint shaderProgID = shader.GetID();

	glUseProgram(shaderProgID);

	// set time
	float time = glfwGetTime();

	glUniform1f(shader["iTime"], time);

	// set resolution
	glUniform2f(shader["iResolution"],
		width,
		height);

	glUniform1i(shader["isWater"],
		false);

	glUniformMatrix4fv(shader["matView"], 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(shader["matProj"], 1, GL_FALSE, glm::value_ptr(p));

	this->DrawObject(objPtr, v, p);
}

void RenderingEngine::RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO)
{
	float width = previousFBO.width;
	float height = previousFBO.height;

	// 1. Disable the FBO

	// Draw to the frame buffer
	fbo.use();
	//fbo.clear_all();

	// 2. Clear the ACTUAL screen buffer
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3. Use the FBO colour texture as the texture on that quad
	GLint shaderProgID = quad.shader->GetID();
	Shader& shader = *quad.shader;
	glUseProgram(shaderProgID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, previousFBO.colourTexture_ID);
	glUniform1i(shader["textSamp00"], 0);	// Texture unit 0		ALBEDO TEXTURE

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, previousFBO.normalTexture_ID);
	glUniform1i(shader["textSamp01"], 1);	// Texture unit 1		NORMAL TEXTURE

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, previousFBO.positionTexture_ID);
	glUniform1i(shader["textSamp02"], 2);	// Texture unit 2		POSITION TEXTURE

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, previousFBO.bloomTexture_ID);
	glUniform1i(shader["textSamp03"], 3);	// Texture unit 3		BLOOM TEXTURE (NOT USED ON THIS PASS)

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, previousFBO.unlitTexture_ID);
	glUniform1i(shader["textSamp04"], 4);	// Texture unit 4		TEXTURE INDICATING UNLIT OBJECTS

	// 4. Draw a single object (a triangle or quad)
	glUniform1i(shader["isFinalPass"], (int)GL_FALSE);

	glUniform2f(shader["iResolution"],
		width,
		height);

	mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	pass_id = 2;

	glUniformMatrix4fv((*quad.shader)["matProj"], 1, GL_FALSE, glm::value_ptr(p));

	this->DrawObject(&quad, mat4(1.f), p);
}

void RenderingEngine::RenderQuadToScreen(cFBO& previousFBO)
{
	float width = previousFBO.width;
	float height = previousFBO.height;

	// LAST RENDER PASS
	GLint shaderProgID = quad.shader->GetID();
	Shader& shader = *quad.shader;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgID);


	glUniform1f(shader["iTime"], (float)glfwGetTime());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, previousFBO.colourTexture_ID);
	glUniform1i(shader["textSamp00"], 0);	// Texture unit 0	LIT SCENE TEXTURE

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, previousFBO.positionTexture_ID);
	glUniform1i(shader["textSamp01"], 1);	// Texture unit 0	POSITION TEXTURE

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, previousFBO.normalTexture_ID);
	glUniform1i(shader["textSamp02"], 2);	// Texture unit 1	LIGHTING DEPTH BUFFER TEXTURE

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, previousFBO.bloomTexture_ID);
	glUniform1i(shader["textSamp03"], 3);	// Texture unit 3	BLOOM CUTOFF TEXTURE

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, noise.GetID());
	glUniform1i(shader["textSamp04"], 4);	// Texture unit 1	NOISE TEXTURE

	glUniform1i(shader["isFinalPass"], (int)GL_TRUE);

	mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	glUniformMatrix4fv(shader["matProj"], 1, GL_FALSE, glm::value_ptr(p));

	if (bloom_enabled)
	{
		glUniform1i(shader["bloomEnabled"], (int)GL_TRUE);
	}
	else
	{
		glUniform1i(shader["bloomEnabled"], (int)GL_FALSE);
	}

	if (DOF_enabled)
	{
		glUniform1i(shader["DOFEnabled"], (int)GL_TRUE);
	}
	else
	{
		glUniform1i(shader["DOFEnabled"], (int)GL_FALSE);
	}

	this->DrawObject(&quad, mat4(1.f), p);
}