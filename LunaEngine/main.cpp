#include <_GL/Window.h>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
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
#include <TextureManager/cBasicTextureManager.h>
#include <Particles/cParticleEffect.h>
#include <Misc/cLowpassFilter.h>
#include <Lua/cLuaBrain.h>
#include <Commands/cMoveTo.h>
#include <Commands/cRotateTo.h>
#include <Commands/cFollowCurve.h>
#include <FBO/cFBO.h>
#include <Components/cMaterial.h>
#include <Components/cRigidBody.h>
#include <Behaviour/cBehaviourManager.h>
#include <EntityManager/cEntityManager.h>
#include <Behaviour/Controls/cSphereBehaviour.h>
#include <Physics/global_physics.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define CAMERA_CONTROL
#define _DEBUG
#define DEFERRED_RENDERING

cFBO* pTheFBO = NULL;

unsigned int input_id = 0;

bool is_paused = false;
int pass_id;

int ball_id = 0;

Scene* scene;
GLFWwindow* global::window = 0;
iInputHandler* pInputHandler;

cBasicTextureManager* textureManager;

void DrawObject(cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p);
void DrawParticle(sParticle* objPtr, glm::mat4 const& v, glm::mat4 const& p);

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		delete pInputHandler;
		input_id++;
		if (input_id % 3 == 0)
		{
			pInputHandler = 0;// new cPhysicsInputHandler(*scene, window);
		}
		else if (input_id % 3 == 1)
		{
			pInputHandler = 0;// new cLightController(*scene);
		}
		else
		{
			pInputHandler = 0;// new cLayoutController(*scene);
		}
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		is_paused = !is_paused;
	}

	if (key == GLFW_KEY_TAB && action == GLFW_RELEASE)
	{
		scene->vecGameObjects[ball_id]->RemoveComponent<cSphereBehaviour>();
		ball_id++;
		if (ball_id == scene->vecGameObjects.size())
			ball_id = 0;
		while (scene->vecGameObjects[ball_id]->tag != "ball")
		{
			ball_id++;
			if (ball_id == scene->vecGameObjects.size())
				ball_id = 0;
		}
		scene->vecGameObjects[ball_id]->AddComponent<cSphereBehaviour>();
	}

	if (pInputHandler) pInputHandler->key_callback(window, key, scancode, action, mods);
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void HandleInput(GLFWwindow* window)
{
#ifdef CAMERA_CONTROL
	glm::vec3 forwardVector = Mathf::get_direction_vector(scene->camera.Eye, scene->camera.Target);
	glm::vec3 backwardsVector = Mathf::get_reverse_direction_vector(scene->camera.Eye, scene->camera.Target);
	glm::vec3 rightVector = Mathf::get_rotated_vector(-90.f, glm::vec3(0.f), forwardVector);
	glm::vec3 leftVector = Mathf::get_rotated_vector(90.f, glm::vec3(0.f), forwardVector);


	// Move the camera (A & D for left and right, along the x axis)
	if (glfwGetKey(window, GLFW_KEY_A))
	{
		Mathf::rotate_vector(-5.f, scene->camera.Target, scene->camera.Eye);
	}
	if (glfwGetKey(window, GLFW_KEY_D))
	{
		Mathf::rotate_vector(5.f, scene->camera.Target, scene->camera.Eye);
	}

	// Move the camera (Q & E for up and down, along the y axis)
	if (glfwGetKey(window, GLFW_KEY_Q))
	{
		scene->camera.Eye.y -= glm::length(forwardVector) * 0.05f;
	}
	if (glfwGetKey(window, GLFW_KEY_E))
	{
		scene->camera.Eye.y += glm::length(forwardVector) * 0.05f;
	}

	// Move the camera (W & S for towards and away, along the z axis)
	if (glfwGetKey(window, GLFW_KEY_W))
	{
		scene->camera.Eye += forwardVector * 0.1f;
	}
	if (glfwGetKey(window, GLFW_KEY_S))
	{
		scene->camera.Eye += backwardsVector * 0.1f;
	}
#endif

	if (pInputHandler) pInputHandler->HandleInput(window);
	
}

int main(void)
{

	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	global::window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLEngine", NULL, NULL);
	GLFWwindow* window = global::window;

	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	typedef glm::vec3 vec3;

	// Initialize physics
	InitPhysics();

	// Load scene from file
	cGameObject* pSkyBoxSphere = new cGameObject();

	textureManager = cBasicTextureManager::Instance();
	std::string errorString;
	textureManager->SetBasePath("assets/textures/cubemaps/");
	if (textureManager->CreateCubeTextureFromBMPFiles("space",
													  "SpaceBox_right1_posX.bmp", "SpaceBox_left2_negX.bmp",
													  "SpaceBox_top3_posY.bmp", "SpaceBox_bottom4_negY.bmp",
													  "SpaceBox_front5_posZ.bmp", "SpaceBox_back6_negZ.bmp", true, errorString))
	{
		scene = Scene::LoadFromXML("sandbox.scene.xml");
		pSkyBoxSphere->transform.pos = vec3(0.f);
		pSkyBoxSphere->meshName = "sphere";
		pSkyBoxSphere->shaderName = "basic";
		pSkyBoxSphere->tag = "skybox";
		pSkyBoxSphere->transform.pos = vec3(0.0f, 0.f, 0.0f);
		pSkyBoxSphere->transform.scale = vec3(900.0f);
		pSkyBoxSphere->texture[0] = "Pizza.bmp";
		pSkyBoxSphere->textureRatio[0] = 1.0f;
		//pSkyBoxSphere->inverseMass = 0.0f;
		//pSkyBoxSphere->gravityScale = 0.f;
		scene->vecGameObjects.push_back(pSkyBoxSphere);
		std::cout << "Space skybox loaded" << std::endl;
	}
	else
	{
		std::cout << "OH NO! Error while loading cubemap :( " << errorString << std::endl;
		exit(1);
	}

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing


	//PhysicsEngine* phys = PhysicsEngine::Instance();
	//phys->GenerateAABB(scene);

	pInputHandler = 0;// new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cLayoutController(*scene);


	cDebugRenderer* renderer = cDebugRenderer::Instance();
	renderer->initialize();

#ifdef _DEBUG
	//phys->renderer = renderer;
#endif

	


	glm::vec3 min = scene->pModelLoader->min;
	glm::vec3 max = scene->pModelLoader->max;

	cGameObject* bounds = new cGameObject;
	bounds->transform.pos = (max + min) / 2.f;
	bounds->transform.scale = vec3(glm::distance(max, min) / 2.f);
	bounds->colour = glm::vec4(1.f, 0.f, 0.f, 1.f);
	bounds->meshName = "cube";
	bounds->tag = "AABB";
	bounds->shaderName = "basic";
	bounds->isWireframe = true;
	bounds->uniformColour = true;
	
	
	//scene->cameraEye = glm::vec3(-39.f, 2.f, -63.f);


	cGameObject* ship = scene->vecGameObjects[0];
	//ship->GetComponent<cRigidBody>()->gravityScale = 1.f;
	sLight* light1 = scene->pLightManager->Lights[0];
	sLight* light2 = scene->pLightManager->Lights[1];

	cParticleEffect pEffect(200);
	pEffect.min_time = .1f;
	pEffect.max_time = 3.f;
	pEffect.min_scale = 0.001f;
	pEffect.max_scale = 0.03f;
	pEffect.pos = vec3(0.f, 20.f, 0.f);
	pEffect.min_vel = vec3(-.3f);
	pEffect.max_vel = vec3(.3f);
	pEffect.Generate();

	cGameObject* sphere = new cGameObject;
	sphere->meshName = "sphere_particle";
	sphere->shaderName = "particle";
	//sphere->inverseMass = 0.f;
	sphere->texture[0] = ship->texture[0];
	sphere->textureRatio[0] = 1.f;

	cGameObject quad;
	quad.meshName = "screen_quad";
	quad.shaderName = "post";

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

	cLowpassFilter* filter = cLowpassFilter::Instance();
	float current_time = (float)glfwGetTime();
	float previous_time = (float)glfwGetTime();
	float delta_time = 0.f;

	scene->camera.Eye = vec3(0.f, 100.f, -200.f);
	scene->camera.Eye = vec3(0, 0, -3);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

#ifdef DEFERRED_RENDERING

	// Set up frame buffer
	cFBO* fbo = new cFBO;
	cFBO* fbo2 = new cFBO;
	std::string fbo_error;
	if (fbo->init(width, height, fbo_error))
	{
		printf("Frame buffer 1 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}
	if (fbo2->init(width, height, fbo_error))
	{
		printf("Frame buffer 2 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}
#endif

	//scene->vecGameObjects[0]->AddComponent<cSphereBehaviour>();

	// MUST COME BEFORE THE START METHODS
	cEntityManager::Instance()->SetEntities(&scene->vecGameObjects);

	// Run the start method on all behaviour components
	cBehaviourManager::Instance()->start();


	while (!glfwWindowShouldClose(window))
	{
#ifdef DEFERRED_RENDERING

		if (!fbo->reset(width, height, errorString))
		{
			std::cout << "fbo was unable to be reset..." << std::endl;
			exit(1);
		}

		// Draw to the frame buffer
		fbo->use();

		//fbo->clearBuffers(true, true);
#endif
		pass_id = 1;


		previous_time = current_time;
		current_time = (float)glfwGetTime();

		if (!is_paused)
		{
			delta_time = filter->add_time(current_time - previous_time);
		}
		else
		{
			delta_time = 0.f;
		}

		// Handle key inputs
		HandleInput(window);

		//scene->cameraTarget = ship->pos;

		float ratio;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;


		glViewport(0, 0, width, height);

		// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//glClearColor(0.7f, 0.85f, 1.f, 1.f);
		//glClearColor(0.7f, 0.85f, 1.f, 1.f);
		//glClearColor(0.f, 0.f, 0.f, 1.f);

		glEnable(GL_BLEND);      // Enable blend or "alpha" transparency
		//glDisable( GL_BLEND );
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		cBehaviourManager::Instance()->update(delta_time);
		cEntityManager::Instance()->Update(delta_time);

		g_PhysicsWorld->Update(delta_time);

		// Update the objects' physics
		//phys->CheckCollisions(scene, delta_time);
		//phys->IntegrationStep(scene, delta_time);

		// Update particles;
		pEffect.Step(delta_time);
		
		// Update 3D audio engine
		//scene->pAudioEngine->Update3d(scene->cameraEye, scene->cameraTarget, delta_time);

		pSkyBoxSphere->transform.pos = scene->camera.Eye;
		// **************************************************
		// **************************************************
		glm::mat4 p, v;

		// Projection matrix
		p = glm::perspective(0.6f,		// FOV
							 ratio,			// Aspect ratio
							 0.1f,			// Near clipping plane
							 1000.f);		// Far clipping plane

		// View matrix
		//v = glm::mat4(1.0f);

		v = glm::lookAt(scene->camera.Eye,
						scene->camera.Target,
						scene->camera.Up);


		int lastShader = -1;
		
		// Loop to draw everything in the scene
		for (int index = 0; index != scene->vecGameObjects.size(); index++)
		{
#ifdef TRANSPARENCY_SORT
			if (index < scene->vecGameObjects.size() - 1)
			{
				glm::vec3 ObjA = scene->vecGameObjects[index]->pos;
				glm::vec3 ObjB = scene->vecGameObjects[index + 1]->pos;

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

			cGameObject* objPtr = scene->vecGameObjects[index];

			cMaterial* material = objPtr->GetComponent<cMaterial>();
			if (material != nullptr)
			{
				// TODO
			}

			objPtr->cmd_group->Update(delta_time);
			objPtr->brain->Update(delta_time);

			

			GLint shaderProgID = scene->Shaders[objPtr->shaderName];

			// Only switch shaders if needed
			if (lastShader != shaderProgID)
			{
				glUseProgram(shaderProgID);
				lastShader = shaderProgID;

				// set time
				float time = glfwGetTime();
				glUniform1f(glGetUniformLocation(shaderProgID, "iTime"), time);

				// set resolution
				glUniform2f(glGetUniformLocation(shaderProgID, "iResolution"),
							width,
							height);
			}
			

			if (objPtr->tag == "water")
			{
				glUniform1i(glGetUniformLocation(shaderProgID, "isWater"),
							true);
			}
			else
			{
				glUniform1i(glGetUniformLocation(shaderProgID, "isWater"),
							false);
			}

			//if (objPtr->tag == "player")
			//{
			//	glm::mat4 model = objPtr->transform.ModelMatrix();
			//	light1->position = model * glm::vec4(ship->CollidePoints[0], 1.f);
			//	light2->position = model * glm::vec4(ship->CollidePoints[1], 1.f);
			//	pEffect.pos = ((light2->position - light1->position) / 2.f) + light1->position;
			//	/*for (int n = 0; n < objPtr->CollidePoints.size(); ++n)
			//	{
			//		renderer->addLine(objPtr->pos,
			//						  glm::vec3(model * glm::vec4(objPtr->CollidePoints[n], 1.f)),
			//						  glm::vec3(1.f, 0.f, 0.f), .1f);
			//	}*/
			//}

			DrawObject(objPtr, v, p);

		}//for (int index...
		// **************************************************
		// **************************************************


		glUseProgram(scene->Shaders[sphere->shaderName]);
		// set time
		float time = glfwGetTime();
		glUniform1f(glGetUniformLocation(scene->Shaders[sphere->shaderName], "iTime"), time);

		// set resolution
		glUniform2f(glGetUniformLocation(scene->Shaders[sphere->shaderName], "iResolution"),
					width,
					height);
		for (unsigned int n = 0; n < pEffect.particles.size(); ++n)
		{
			sParticle* particle = pEffect.particles[n];
			sphere->transform.pos = particle->pos;
			sphere->transform.scale = vec3(particle->scale);
			DrawObject(sphere, v, p);
		}


		
		//DrawOctree(ship, phys->tree->main_node, bounds, ratio, v, p);

		// **************************************************
		// **************************************************
		renderer->RenderDebugObjects(v, p, 0.01f);


#ifdef DEFERRED_RENDERING
		// 1. Disable the FBO
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (!fbo2->reset(width, height, errorString))
		{
			std::cout << "fbo was unable to be reset..." << std::endl;
			exit(1);
		}

		// Draw to the frame buffer
		fbo2->use();

		// 2. Clear the ACTUAL screen buffer
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 3. Use the FBO colour texture as the texture on that quad
		//cGameObject* pQuad = scene->vecGameObjects[2];
		GLint shaderProgID = scene->Shaders[quad.shaderName];
		glUseProgram(shaderProgID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo->colourTexture_0_ID);
		glUniform1i(glGetUniformLocation(shaderProgID, "textSamp00"), 0);	// Texture unit 0

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fbo->normalTexture_ID);
		glUniform1i(glGetUniformLocation(shaderProgID, "textSamp01"), 1);	// Texture unit 1

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, fbo->positionTexture_ID);
		glUniform1i(glGetUniformLocation(shaderProgID, "textSamp02"), 2);	// Texture unit 2

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, fbo->bloomTexture_ID);
		glUniform1i(glGetUniformLocation(shaderProgID, "textSamp03"), 3);	// Texture unit 3

		// 4. Draw a single object (a triangle or quad)
		glUniform1i(glGetUniformLocation(shaderProgID, "isFinalPass"), (int)GL_FALSE);

		p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
		pass_id = 2;

		DrawObject(&quad, v, p);


		// LAST RENDER PASS

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shaderProgID);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo2->colourTexture_0_ID);
		glUniform1i(glGetUniformLocation(shaderProgID, "textSamp00"), 0);	// Texture unit 0

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, fbo2->bloomTexture_ID);
		glUniform1i(glGetUniformLocation(shaderProgID, "textSamp03"), 3);	// Texture unit 3

		glUniform1i(glGetUniformLocation(shaderProgID, "isFinalPass"), (int)GL_TRUE);

		DrawObject(&quad, v, p);

		/*pQuad = scene->vecGameObjects[1];
		shaderProgID = scene->Shaders[pQuad->shaderName];
		glUseProgram(shaderProgID);
		pass_id = 1;
		DrawObject(pQuad, ratio, v, p);*/
		
#endif
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();

	ReleasePhysics();
	// Delete everything
	delete scene;
	//delete lightManager;

	exit(EXIT_SUCCESS);
}


void SetUpTextureBindingsForObject(
	cGameObject* pCurrentObject,
	GLint shaderProgID)
{
	if (pass_id != 1) return;
	//// Tie the texture to the texture unit
	//GLuint texSamp0_UL = ::g_pTextureManager->getTextureIDFromName("Pizza.bmp");
	//glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	//glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	// Tie the texture to the texture unit
	GLuint texSamp0_UL = textureManager->getTextureIDFromName(pCurrentObject->texture[0]);
	glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp1_UL = ::textureManager->getTextureIDFromName(pCurrentObject->texture[1]);
	glActiveTexture(GL_TEXTURE1);				// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, texSamp1_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp2_UL = ::textureManager->getTextureIDFromName(pCurrentObject->texture[2]);
	glActiveTexture(GL_TEXTURE2);				// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, texSamp2_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp3_UL = ::textureManager->getTextureIDFromName(pCurrentObject->texture[3]);
	glActiveTexture(GL_TEXTURE3);				// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, texSamp3_UL);	// Texture now assoc with texture unit 0

	// Tie the texture units to the samplers in the shader
	GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
	glUniform1i(textSamp00_UL, 0);	// Texture unit 0

	GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
	glUniform1i(textSamp01_UL, 1);	// Texture unit 1

	GLint textSamp02_UL = glGetUniformLocation(shaderProgID, "textSamp02");
	glUniform1i(textSamp02_UL, 2);	// Texture unit 2

	GLint textSamp03_UL = glGetUniformLocation(shaderProgID, "textSamp03");
	glUniform1i(textSamp03_UL, 3);	// Texture unit 3


	GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_3_ratio");
	glUniform4f(tex0_ratio_UL,
				pCurrentObject->textureRatio[0],		// 1.0
				pCurrentObject->textureRatio[1],
				pCurrentObject->textureRatio[2],
				pCurrentObject->textureRatio[3]);



	return;
}


void DrawObject(cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p)
{
	GLint shaderProgID = scene->Shaders[objPtr->shaderName];

	GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgID, "isSkybox");
	if (objPtr->tag != "skybox")
	{
		glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
		// Don't draw back facing triangles (default)
		glCullFace(GL_BACK);
		SetUpTextureBindingsForObject(objPtr, shaderProgID);
	}
	else
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		//glCullFace(GL_FRONT_AND_BACK);

		glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);

		GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
		glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		// Tie the texture units to the samplers in the shader
		GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgID, "skyBox");
		glUniform1i(skyBoxSampler_UL, 26);	// Texture unit 26
	}

	glm::mat4 m;

	m = glm::mat4(1.0f);



	// ******* TRANSLATION TRANSFORM *********
	glm::mat4 matTrans
		= glm::translate(glm::mat4(1.0f),
						 glm::vec3(objPtr->transform.pos.x,
								   objPtr->transform.pos.y,
								   objPtr->transform.pos.z));
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********



	// ******* ROTATION TRANSFORM *********
	//mat4x4_rotate_Z(m, m, (float) glfwGetTime());
	//glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
	//								objPtr->rotation.z,					// Angle
	//								glm::vec3(0.0f, 0.0f, 1.0f));
	//m = m * rotateZ;

	//glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
	//								objPtr->rotation.y,	//(float)glfwGetTime(),					// Angle
	//								glm::vec3(0.0f, 1.0f, 0.0f));
	//m = m * rotateY;

	//glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
	//								objPtr->rotation.x,	// (float)glfwGetTime(),					// Angle
	//								glm::vec3(1.0f, 0.0f, 0.0f));
	m = m * glm::mat4(objPtr->transform.rotation);
	// ******* ROTATION TRANSFORM *********

	if (pass_id != 1) m = glm::mat4(1.f);

	GLint matModelIT_UL = glGetUniformLocation(shaderProgID, "matModelInverTrans");
	glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	// ******* SCALE TRANSFORM *********
	/*glm::mat4 scale = glm::scale(glm::mat4(1.0f),
								 glm::vec3(objPtr->scale,
										   objPtr->scale,
										   objPtr->scale));
	m = m * scale;*/
	// ******* SCALE TRANSFORM *********
	

	GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");
	GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
	GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(objPtr->transform.ModelMatrix()));
	glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(v));
	glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(p));

	

	GLint eye_loc = glGetUniformLocation(shaderProgID, "eyeLocation");
	glUniform4f(eye_loc,
				scene->camera.Eye.x,
				scene->camera.Eye.y,
				scene->camera.Eye.z,
				1.0f
	);

	GLint diff_loc = glGetUniformLocation(shaderProgID, "diffuseColour");
	glUniform4f(diff_loc,
				objPtr->colour.x,
				objPtr->colour.y,
				objPtr->colour.z,
				objPtr->colour.w
	);

	GLint spec_loc = glGetUniformLocation(shaderProgID, "specularColour");
	glUniform4f(spec_loc,
				objPtr->specColour.x,
				objPtr->specColour.y,
				objPtr->specColour.z,
				objPtr->specIntensity
	);

	GLint isUniform_location = glGetUniformLocation(shaderProgID, "isUniform");
	glUniform1i(isUniform_location, objPtr->uniformColour);


	if (pass_id != 1)
	{
		scene->pLightManager->Set_Light_Data(shaderProgID);
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
	if (scene->pVAOManager->FindDrawInfoByModelName(objPtr->meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(GL_TRIANGLES,
					   drawInfo.numberOfIndices,
					   GL_UNSIGNED_INT,
					   0);
		glBindVertexArray(0);
	}
}

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p)
{
	if (node == nullptr) return;

	if (!node->AABB->contains(obj->transform.pos)) return;

	
	if (!node->has_nodes)
	{
		glUniform1i(glGetUniformLocation(scene->Shaders[obj->shaderName], "isWater"),
			false);

		objPtr->transform.pos = (node->AABB->min + (node->AABB->min + node->AABB->length)) / 2.f;
		objPtr->transform.scale = glm::vec3(node->AABB->length / 2.f);

		DrawObject(objPtr, v, p);
	}

	for (int i = 0; i < 8; ++i)
		DrawOctree(obj, node->nodes[i], objPtr, v, p);

}

void DrawParticle(sParticle* objPtr, glm::mat4 const& v, glm::mat4 const& p)
{

}