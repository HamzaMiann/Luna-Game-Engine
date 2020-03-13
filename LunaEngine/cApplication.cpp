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

iApplication* cApplication::app = cApplication::Instance();


std::string errorString;

bool is_paused = false;

Scene* scene;
iInputHandler* pInputHandler;

cFBO albedoFBO;
cFBO second_passFBO;
cFBO finalFBO;
cFBO blur_fbo;

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
	scene = Scene::LoadFromXML("AI2.scene.xml");
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
	

	cLowpassFilter& filter = cLowpassFilter::Instance();
	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float dt = 0.f;

	sLight* light = cLightManager::Instance()->Lights[0];
	vec3 origin = vec3(light->position);


	while (!glfwWindowShouldClose(global::window))
	{
		renderer.pass_id = 1;

		previous_time = current_time;
		current_time = (float)glfwGetTime();

		if (!is_paused) dt = filter.add_time(current_time - previous_time);
		else dt = 0.f;

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

		renderer.Reset();

		behaviour_manager.update(dt);
		entity_manager.Update(dt);

		g_PhysicsWorld->Update(dt);

		// Update 3D audio engine
		//scene->pAudioEngine->Update3d(scene->cameraEye, scene->cameraTarget, delta_time);

		// Move skybox relative to the camera
		renderer.skyBox.transform.pos = Camera::main_camera->Eye;

		mat4 p, v;

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

		renderer.RenderObjectsToFBO(&second_passFBO, width, height, p, v, dt);
		renderer.RenderSkybox(width, height, p, v, dt);

		light->position = vec4(origin + Camera::main_camera->Eye, 1.0f);
		renderer.screenPos = vec2((p * v * mat4(1.0f)) * light->position);

		renderer.RenderQuadToFBO(finalFBO, second_passFBO);
		renderer.RenderQuadToScreen(finalFBO);

		debug_renderer.RenderDebugObjects(v, p, dt);

		Input::ClearBuffer();

		glfwSwapBuffers(global::window);
		glfwPollEvents();
	}
}

//void RenderText(Shader& s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
//{
//	// Activate corresponding render state	
//	s.Use();
//	glUniform3f(glGetUniformLocation(s.Program, "textColor"), color.x, color.y, color.z);
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(VAO);
//
//	// Iterate through all characters
//	std::string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++)
//	{
//		Character ch = Characters[*c];
//
//		GLfloat xpos = x + ch.Bearing.x * scale;
//		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
//		GLfloat w = ch.Size.x * scale;
//		GLfloat h = ch.Size.y * scale;
//		// Update VBO for each character
//		GLfloat vertices[6][4] = {
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos,     ypos,       0.0, 1.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//			{ xpos + w, ypos + h,   1.0, 0.0 }
//		};
//		// Render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.textureID);
//		// Update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		// Render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//		x += (ch.Advance >> 6)* scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}



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
