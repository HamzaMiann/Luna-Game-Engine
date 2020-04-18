#include "cApplication.h"
#include <_GL/Window.h>
#include <glm/glm_common.h>
#include <string>

#include <Scene/Scene.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <cGameObject.h>

#include <Lighting/cLightManager.h>
#include <InputHandlers/cPhysicsInputHandler.h>
#include <InputHandlers/cLayoutController.h>
#include <InputHandlers/cLightController.h>
#include <Misc/cLowpassFilter.h>
#include <Lua/cLuaBrain.h>
#include <FBO/cFBO.h>
#include <Behaviour/cBehaviourManager.h>
#include <EntityManager/cEntityManager.h>
#include <Physics/global_physics.h>
#include <InputManager.h>
#include <Rendering/RenderingEngine.h>
#include <Physics/Mathf.h>
#include <Threading/threading.h>
#include <Mesh\cVAOManager.h>

iApplication* cApplication::app = cApplication::Instance();


std::string errorString;

bool is_paused = false;

Scene* scene;
iInputHandler* pInputHandler;

cFBO albedoFBO;
cFBO second_passFBO;
cFBO finalFBO;

void InitWindow()
{
	// Initialize GL
	SetErrorCallback(error_callback);
	InitGL();
	SetKeyCallback(key_callback);
	SetScrollCallback(scroll_callback);
	SetDropCallback(drop_callback);
	glfwSetMouseButtonCallback(global::window, mouse_button_callback);
}

void RunScripts()
{
	//cLuaBrain::SetObjectVector(&scene->vecGameObjects);
	/*cLuaBrain::SetCamera(Camera::main_camera);

	for (int index = 0; index != scene->vecGameObjects.size(); index++)
	{
		cGameObject* objPtr = scene->vecGameObjects[index];
		if (objPtr->lua_script != "")
		{
			objPtr->brain->RunScriptImmediately(objPtr->lua_script);
		}
	}*/
}

void cApplication::Init()
{
	// Create Window
	InitWindow();

	// Initialize physics
	InitPhysics();

	// Load scene from file
	scene = Scene::LoadFromXML("sandbox2.scene.xml");
	scene->camera.Eye = vec3(0.f, 100.f, -200.f);
	scene->camera.Eye = vec3(0, 0, -3);

	pInputHandler = 0;
	//pInputHandler = new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cLayoutController(*scene);

	// Run lua scripts on objects
	RunScripts();

}

struct plane
{
	vec3 normal;
	vec3 pos;
	float constant;
};

void cApplication::Run()
{
	RenderingEngine& renderer = RenderingEngine::Instance();
	cDebugRenderer& debug_renderer = cDebugRenderer::Instance();
	cEntityManager& entity_manager = cEntityManager::Instance();
	cBehaviourManager& behaviour_manager = cBehaviourManager::Instance();


	int width, height;
	glfwGetFramebufferSize(global::window, &width, &height);

	// Set up frame buffers
	std::string fbo_error;
	if (albedoFBO.init(width, height, fbo_error)
		&&
		second_passFBO.init(width, height, fbo_error)
		&&
		finalFBO.init(width, height, fbo_error)) printf("Frame buffers are OK\n");
	else
	{
		printf("Frame buffers broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}

	// Run the start method on all behaviour components
	behaviour_manager.start();
	// Initialize renderer
	renderer.Init();
	// Initialize debug renderer
	debug_renderer.initialize();

	for (int i = 0; i < 1000; ++i)
	{
		cGameObject* obj = new cGameObject;
		obj->meshName = "grass";
		obj->shader = Shader::FromName("basic");
		obj->transform.Position(
			vec3(
				Mathf::randInRange(-150.f, 150.f),
				0.0f,
				Mathf::randInRange(-150.f, 150.f)
				)
			);
		obj->transform.UpdateEulerRotation(
			vec3(
				0.f,
				Mathf::randInRange(0.f, 360.f),
				0.f
				)
			);
		obj->transform.Scale(vec3(0.1f));
		obj->texture[0].SetTexture("Grass.png", 1.f);
		entity_manager.AddEntity(obj);
	}
	

	cLowpassFilter& filter = cLowpassFilter::Instance();
	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float dt = 0.f;

	sLight* light = cLightManager::Instance()->Lights[0];
	vec3 origin = vec3(light->position);

	g_PhysicsWorld->SetDebugRenderer(&renderer);

	entity_manager.GetObjectByTag("cube")->GetComponent<nPhysics::iPhysicsComponent>()->AddHingeConstraint(vec3(0.f, 2.f, 0.f), vec3(0.f, 1.f, 0.f));
	//entity_manager.GetObjectByTag("cube")->GetComponent<nPhysics::iPhysicsComponent>()->AddSliderConstraint(vec2(-15.f, -5.f), vec2(-PI / 3.0f, PI / 3.0f));
	//entity_manager.GetObjectByTag("cube")->GetComponent<nPhysics::iPhysicsComponent>()->AddSliderConstraint(vec2(0.f, 0.f), vec2(-PI / 3.0f, PI / 3.0f));

	float maxTime = 1.f;

	

	while (!glfwWindowShouldClose(global::window))
	{
		renderer.pass_id = 1;

		previous_time = current_time;
		current_time = (float)glfwGetTime();

		if (!is_paused) dt = filter.add_time(current_time - previous_time);
		else dt = 0.f;

		int newWidth, newHeight;
		glfwGetFramebufferSize(global::window, &newWidth, &newHeight);

		if (newWidth != width || newHeight != height)
		{
			albedoFBO.reset(newWidth, newHeight, fbo_error);
			second_passFBO.reset(newWidth, newHeight, fbo_error);
			finalFBO.reset(newWidth, newHeight, fbo_error);
		}

		width = newWidth;
		height = newHeight;
		//glViewport(0, 0, width, height);

		renderer.Reset();

		Thread::Update(dt);

		behaviour_manager.update(dt);

		entity_manager.Update(dt);

		g_PhysicsWorld->Update(dt);

		renderer.mDt = dt;

		if (Input::KeyDown(GLFW_KEY_TAB))
		{
			g_PhysicsWorld->ToggleDebugMode();
		}

		if (renderer.GetBoolProperty("shadowsEnabled"))
			renderer.RenderObjectsToFBO(&albedoFBO, width, height, dt, true);

		// Update 3D audio engine
		//scene->pAudioEngine->Update3d(scene->cameraEye, scene->cameraTarget, delta_time);

		// Move skybox relative to the camera
		renderer.skyBox.transform.pos = Camera::main_camera->Eye;

		mat4 p, v;

		float ratio = width / (float)height;
		// Projection matrix
		p = glm::perspective(	0.6f,			// FOV
								ratio,			// Aspect ratio
								0.1f,			// Near clipping plane
								1000.f	);		// Far clipping plane

		v = glm::lookAt(
			Camera::main_camera->Eye,
			Camera::main_camera->Target,
			Camera::main_camera->Up
		);

		renderer.projection = p;
		renderer.view = v;

		light->position = vec4(origin + Camera::main_camera->Eye, 1.0f);
		vec4 homogenous = (p * v * mat4(1.0f)) * light->position;
		vec3 cube = vec3(homogenous.x / homogenous.w, homogenous.y / homogenous.w, homogenous.z / homogenous.w);
		renderer.screenPos = vec2(cube);

		renderer.RenderObjectsToFBO(&second_passFBO, width, height, dt, false);
		renderer.RenderSkybox(width, height, p, v, dt);

		renderer.RenderLightingToFBO(finalFBO, second_passFBO, albedoFBO.depthTexture_ID);
		renderer.RenderPostProcessingToScreen(finalFBO, second_passFBO.unlitTexture_ID);

		renderer.DrawDebugObjects();
		debug_renderer.RenderDebugObjects(v, p, dt);

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

	cEntityManager::Instance().Release();

	delete scene;
	ReleasePhysics();

	glfwTerminate();
}
