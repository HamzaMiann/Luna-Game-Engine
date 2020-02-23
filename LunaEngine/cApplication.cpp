#include "cApplication.h"
#include <_GL/Window.h>
#include <glm/glm_common.h>
#include <string>

#include <Physics/Mathf.h>
#include <Physics/PhysicsEngine.h>
#include <Scene/Scene.h>
#include <Lighting/cLightManager.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <cGameObject.h>
#include <Mesh/cVAOManager.h>

#include <InputHandlers/cPhysicsInputHandler.h>
#include <InputHandlers/cLayoutController.h>
#include <InputHandlers/cLightController.h>
#include <Mesh/cModelLoader.h>
#include <Audio/AudioEngine.hpp>
#include <Texture/cBasicTextureManager.h>
#include <Misc/cLowpassFilter.h>
#include <Lua/cLuaBrain.h>
#include <FBO/cFBO.h>
#include <Components/cMaterial.h>
#include <Behaviour/cBehaviourManager.h>
#include <EntityManager/cEntityManager.h>
#include <Physics/global_physics.h>
#include <Animation/cAnimationManager.h>
#include <InputManager.h>
#include <Components/cAnimationController.h>
#include <Behaviour/Controls/cCharacterController.h>

iApplication* cApplication::app = cApplication::Instance();


std::string errorString;
cGameObject quad;

bool is_paused = false;
int pass_id;

bool bloom_enabled = true;
bool DOF_enabled = true;

Scene* scene;
iInputHandler* pInputHandler;

cGameObject* pSkyBoxSphere;
std::string skyboxName = "sky";

cFBO albedoFBO;
cFBO second_passFBO;
cFBO finalFBO;
cFBO blur_fbo;

cTexture noise;

vec2 screenPos(0.f, 0.f);

void DrawObject(cGameObject* objPtr, mat4 const& v, mat4 const& p);

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p);
void RenderGO(cGameObject* object, float width, float height, mat4& p, mat4& v, int& lastShader);
void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, mat4 p, mat4 v, float dt);
void RenderSkybox(float width, float height, mat4 p, mat4 v, float dt);
void RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO);
void RenderQuadToScreen(cFBO& previousFBO);

void cApplication::Init()
{
	// Initialize GL
	SetErrorCallback(error_callback);
	InitGL();
	SetKeyCallback(key_callback);
	GLFWwindow* window = global::window;

	// Initialize physics
	InitPhysics();

	// Load scene from file
	scene = Scene::LoadFromXML("sandbox.scene.xml");

	pSkyBoxSphere = new cGameObject();
	pSkyBoxSphere->transform.pos = vec3(0.f);
	pSkyBoxSphere->meshName = "sphere";
	pSkyBoxSphere->shader = Shader::FromName("basic");
	pSkyBoxSphere->tag = "skybox";
	pSkyBoxSphere->transform.scale = vec3(900.0f);



	pInputHandler = 0;// new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cLayoutController(*scene);

	//vec3 min = scene->pModelLoader->min;
	//vec3 max = scene->pModelLoader->max;

	quad.meshName = "screen_quad";
	quad.shader = Shader::FromName("post");

	cLuaBrain::SetObjectVector(&scene->vecGameObjects);
	cLuaBrain::SetCamera(Camera::main_camera);

	for (int index = 0; index != scene->vecGameObjects.size(); index++)
	{
		cGameObject* objPtr = scene->vecGameObjects[index];
		if (objPtr->lua_script != "")
		{
			objPtr->brain->RunScriptImmediately(objPtr->lua_script);
		}
	}

	scene->camera.Eye = vec3(0.f, 100.f, -200.f);
	scene->camera.Eye = vec3(0, 0, -3);
}

void cApplication::Run()
{
	cDebugRenderer& debug_renderer = cDebugRenderer::Instance();
	cEntityManager& entity_manager = cEntityManager::Instance();
	cBehaviourManager& behaviour_manager = cBehaviourManager::Instance();


	debug_renderer.initialize();

	int width, height;
	glfwGetFramebufferSize(global::window, &width, &height);

	// Set up frame buffer
	std::string fbo_error;
	if (albedoFBO.init(width, height, fbo_error))
	{
		printf("Frame buffer 1 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}
	if (second_passFBO.init(width, height, fbo_error))
	{
		printf("Frame buffer 2 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}
	if (finalFBO.init(width, height, fbo_error))
	{
		printf("Frame buffer 3 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}

	// Run the start method on all behaviour components
	behaviour_manager.start();

	cGameObject* screen = entity_manager.GetGameObjectByTag("scope");

	unsigned int charIndex = 0;
	cGameObject* char1 = entity_manager.GetGameObjectByTag("character1");
	cGameObject* char2 = entity_manager.GetGameObjectByTag("character2");
	cGameObject* portal = entity_manager.GetGameObjectByTag("portal");
	cGameObject* portal2 = entity_manager.GetGameObjectByTag("portal2");
	cGameObject* ground = entity_manager.GetGameObjectByTag("ground");
	noise.SetTexture("noise.jpg");

	cLowpassFilter& filter = cLowpassFilter::Instance();
	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float delta_time = 0.f;

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	sLight* light = cLightManager::Instance()->Lights[0];
	vec3 origin = vec3(light->position);

	while (!glfwWindowShouldClose(global::window))
	{
		pass_id = 1;

		previous_time = current_time;
		current_time = (float)glfwGetTime();

		if (!is_paused)
		{
			delta_time = filter.add_time(current_time - previous_time);
		}
		else
		{
			delta_time = 0.f;
		}

		int newWidth, newHeight;
		glfwGetFramebufferSize(global::window, &newWidth, &newHeight);
		float ratio = width / (float)height;

		if (newWidth != width || newHeight != height)
		{
			albedoFBO.reset(newWidth, newHeight, fbo_error);
			second_passFBO.reset(newWidth, newHeight, fbo_error);
			finalFBO.reset(newWidth, newHeight, fbo_error);
		}

		width = newWidth;
		height = newHeight;
		glViewport(0, 0, width, height);



		if (Input::KeyUp(GLFW_KEY_TAB))
		{
			if (charIndex == 0)
			{
				charIndex = 1;
				auto settings = char1->GetComponent<cCharacterController>()->GetSettings();
				char1->GetComponent<cCharacterController>()->OnDestroy();
				char1->RemoveComponent<cCharacterController>();
				cCharacterController* controller = char2->AddComponent<cCharacterController>();
				controller->SetSettings(settings);
			}
			else
			{
				charIndex = 0;
				auto settings = char2->GetComponent<cCharacterController>()->GetSettings();
				char2->GetComponent<cCharacterController>()->OnDestroy();
				char2->RemoveComponent<cCharacterController>();
				cCharacterController* controller = char1->AddComponent<cCharacterController>();
				controller->SetSettings(settings);
			}
		}



		behaviour_manager.update(delta_time);
		entity_manager.Update(delta_time);

		g_PhysicsWorld->Update(delta_time);

		// Update 3D audio engine
		//scene->pAudioEngine->Update3d(scene->cameraEye, scene->cameraTarget, delta_time);

		// Move skybox relative to the camera
		pSkyBoxSphere->transform.pos = scene->camera.Eye;

		mat4 p, v;

		// Projection matrix
		p = glm::perspective(	0.6f,			// FOV
								ratio,			// Aspect ratio
								0.1f,			// Near clipping plane
								1000.f	);		// Far clipping plane

		vec3 dir = glm::normalize(vec3(0, 2.f, 0.f) - scene->camera.Eye);

		// View matrix
		v = glm::lookAt(
			vec3(0, 2.f, 0.f),
			vec3(0.f, 2.f, 0.f) + dir,
			scene->camera.Up
		);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	// Enable writing to the colour buffer
		glDepthMask(GL_TRUE);								// Enable writing to the depth buffer
		glEnable(GL_DEPTH_TEST);							// Enable depth testing
		glDisable(GL_STENCIL_TEST);							// Disable stencil test

		RenderObjectsToFBO(&albedoFBO, width, height, p, v, delta_time);
		RenderSkybox(width, height, p, v, delta_time);

		// set TV screen texture
		screen->texture[0].SetTexture(albedoFBO.normalTexture_ID);

		v = glm::lookAt(
			scene->camera.Eye,
			scene->camera.Target,
			scene->camera.Up
		);

		RenderObjectsToFBO(&second_passFBO, width, height, p, v, delta_time);
		RenderSkybox(width, height, p, v, delta_time);


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

		int last = -1;
		RenderGO(portal, width, height, p, v, last);
		RenderGO(portal2, width, height, p, v, last);

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

		int texture = ground->texture[0].GetID();
		ground->texture[0].SetTexture(noise.GetID());
		RenderGO(char1, width, height, p, v, last);
		RenderGO(char2, width, height, p, v, last);
		RenderGO(ground, width, height, p, v, last);
		skyboxName = "pinksky";
		RenderSkybox(width, height, p, v, delta_time);

		ground->texture[0].SetTexture(texture);

		glDisable(GL_STENCIL_TEST);

		skyboxName = "sky";





		light->position = vec4(origin + Camera::main_camera->Eye, 1.0f);
		screenPos = vec2((p * v * mat4(1.0f)) * light->position);

		RenderQuadToFBO(finalFBO, second_passFBO);
		RenderQuadToScreen(finalFBO);


		//DrawOctree(ship, phys->tree->main_node, bounds, ratio, v, p);
		debug_renderer.RenderDebugObjects(v, p, delta_time);


		Input::ClearBuffer();

		glfwSwapBuffers(global::window);
		glfwPollEvents();
	}
}



void cApplication::End()
{
	glfwDestroyWindow(global::window);

	// Delete everything
	albedoFBO.shutdown();
	second_passFBO.shutdown();
	finalFBO.shutdown();
	blur_fbo.shutdown();

	cEntityManager::Instance().Release();

	delete scene;
	ReleasePhysics();

	glfwTerminate();
}






void SetUpTextureBindingsForObject(cGameObject* pCurrentObject)
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


void DrawObject(cGameObject* objPtr, mat4 const& v, mat4 const& p)
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
	
	glUniform4f(shader["eyeLocation"],
		scene->camera.Eye.x,
		scene->camera.Eye.y,
		scene->camera.Eye.z,
		1.0f
	);

	glUniform4f(shader["eyeTarget"],
		scene->camera.Target.x,
		scene->camera.Target.y,
		scene->camera.Target.z,
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

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, mat4 const& v, mat4 const& p)
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

void RenderGO(cGameObject* object, float width, float height, mat4& p, mat4& v, int& lastShader)
{

	for (cGameObject*& child : object->children)
	{
		RenderGO(child, width, height, p, v, lastShader);
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

	DrawObject(object, v, p);
}

void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, mat4 p, mat4 v, float dt)
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

		objPtr->cmd_group->Update(dt);
		objPtr->brain->Update(dt);

		if (objPtr->tag == "portal" || objPtr->tag == "portal2") continue;

		RenderGO(objPtr, width, height, p, v, lastShader);


	}//for (int index...
	// **************************************************

}

void RenderSkybox(float width, float height, mat4 p, mat4 v, float dt)
{
	// RENDER SKYBOX
	cGameObject* objPtr = pSkyBoxSphere;
	Shader& shader = *objPtr->shader;

	cMaterial* material = objPtr->GetComponent<cMaterial>();
	if (material != nullptr)
	{
		// TODO
	}

	objPtr->cmd_group->Update(dt);
	objPtr->brain->Update(dt);

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

	DrawObject(objPtr, v, p);
}

void RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO)
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

	DrawObject(&quad, mat4(1.f), p);
}

void RenderQuadToScreen(cFBO& previousFBO)
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

	DrawObject(&quad, mat4(1.f), p);
}