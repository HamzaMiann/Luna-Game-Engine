#include "cApplication.h"
#include <_GL/Window.h>
#include <glm/glm_common.h>
#include <string>

#include <Physics/Mathf.h>
#include <Physics/PhysicsEngine.h>
#include <Scene/Scene.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <cGameObject.h>

#include <Lighting/cLightManager.h>
#include <InputHandlers/cPhysicsInputHandler.h>
#include <InputHandlers/cLayoutController.h>
#include <InputHandlers/cLightController.h>
#include <Audio/AudioEngine.hpp>
#include <Misc/cLowpassFilter.h>
#include <Lua/cLuaBrain.h>
#include <FBO/cFBO.h>
#include <Behaviour/cBehaviourManager.h>
#include <EntityManager/cEntityManager.h>
#include <Physics/global_physics.h>
#include <InputManager.h>
#include <Behaviour/Controls/cCharacterController.h>
#include <Rendering/RenderingEngine.h>

iApplication* cApplication::app = cApplication::Instance();


std::string errorString;

bool is_paused = false;

Scene* scene;
iInputHandler* pInputHandler;

cFBO albedoFBO;
cFBO second_passFBO;
cFBO finalFBO;
cFBO blur_fbo;

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


	pInputHandler = 0;
	//pInputHandler = new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cLayoutController(*scene);

	

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
	RenderingEngine& renderer = RenderingEngine::Instance();
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

	cTexture noise;
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
		renderer.pass_id = 1;

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
		renderer.skyBox.transform.pos = scene->camera.Eye;

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

		renderer.RenderObjectsToFBO(&albedoFBO, width, height, p, v, delta_time);
		renderer.RenderSkybox(width, height, p, v, delta_time);

		// set TV screen texture
		screen->texture[0].SetTexture(albedoFBO.normalTexture_ID);

		v = glm::lookAt(
			scene->camera.Eye,
			scene->camera.Target,
			scene->camera.Up
		);

		renderer.RenderObjectsToFBO(&second_passFBO, width, height, p, v, delta_time);
		renderer.RenderSkybox(width, height, p, v, delta_time);


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
		renderer.RenderGO(portal, width, height, p, v, last);
		renderer.RenderGO(portal2, width, height, p, v, last);

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
		renderer.RenderGO(char1, width, height, p, v, last);
		renderer.RenderGO(char2, width, height, p, v, last);
		renderer.RenderGO(ground, width, height, p, v, last);
		renderer.skyboxName = "pinksky";
		renderer.RenderSkybox(width, height, p, v, delta_time);

		ground->texture[0].SetTexture(texture);

		glDisable(GL_STENCIL_TEST);

		renderer.skyboxName = "sky";





		light->position = vec4(origin + Camera::main_camera->Eye, 1.0f);
		renderer.screenPos = vec2((p * v * mat4(1.0f)) * light->position);

		renderer.RenderQuadToFBO(finalFBO, second_passFBO);
		renderer.RenderQuadToScreen(finalFBO);


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
