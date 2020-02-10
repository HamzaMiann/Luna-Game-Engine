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
#include <Texture/cBasicTextureManager.h>
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


#define CAMERA_CONTROL

std::string errorString;
cGameObject quad;

unsigned int input_id = 0;

bool is_paused = false;
int pass_id;

bool bloom_enabled = true;
bool DOF_enabled = true;

Scene* scene;
iInputHandler* pInputHandler;

cBasicTextureManager* textureManager;
cGameObject* pSkyBoxSphere;

void DrawObject(cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p);
void DrawParticle(sParticle* objPtr, glm::mat4 const& v, glm::mat4 const& p);

void DrawOctree(cGameObject* obj, octree::octree_node* node, cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p);

void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, glm::mat4 p, glm::mat4 v, float dt);
void RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO);
void RenderQuadToScreen(cFBO& previousFBO);

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

	if (key == GLFW_KEY_B && action == GLFW_RELEASE)
	{
		bloom_enabled = !bloom_enabled;
		DOF_enabled = !DOF_enabled;
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
	typedef glm::vec3 vec3;

	// Initialize GL
	SetErrorCallback(error_callback);
	InitGL();
	SetKeyCallback(key_callback);
	GLFWwindow* window = global::window;


	// Initialize physics
	InitPhysics();

	// Load scene from file
	pSkyBoxSphere = new cGameObject();

	textureManager = cBasicTextureManager::Instance();
	textureManager->SetBasePath("assets/textures/cubemaps/");
	/*if (textureManager->CreateCubeTextureFromPNGFiles("space",
													  "pink_left.png", "pink_right.png",
													  "pink_top.png", "pink_bottom.png",
													  "pink_front.png", "pink_back.png", true, errorString))*/
	if (textureManager->CreateCubeTextureFromJPGFiles("space",
													  "mountains_right.jpg", "mountains_left.jpg",
													  "mountains_top.jpg", "mountains_bottom.jpg",
													  "mountains_front.jpg", "mountains_back.jpg", true, errorString))
	{
		scene = Scene::LoadFromXML("sandbox.scene.xml");
		pSkyBoxSphere->transform.pos = vec3(0.f);
		pSkyBoxSphere->meshName = "sphere";
		pSkyBoxSphere->shader.SetShader("basic");
		pSkyBoxSphere->tag = "skybox";
		pSkyBoxSphere->transform.pos = vec3(0.0f, 0.f, 0.0f);
		pSkyBoxSphere->transform.scale = vec3(900.0f);
		pSkyBoxSphere->texture[0].SetTexture("Pizza.bmp", 1.0f);
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

	

	pInputHandler = 0;// new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cPhysicsInputHandler(*scene, window);
	//pInputHandler = new cLayoutController(*scene);


	cDebugRenderer* renderer = cDebugRenderer::Instance();
	renderer->initialize();

	glm::vec3 min = scene->pModelLoader->min;
	glm::vec3 max = scene->pModelLoader->max;
	

	/*cParticleEffect pEffect(200);
	pEffect.min_time = .1f;
	pEffect.max_time = 3.f;
	pEffect.min_scale = 0.001f;
	pEffect.max_scale = 0.03f;
	pEffect.pos = vec3(0.f, 20.f, 0.f);
	pEffect.min_vel = vec3(-.3f);
	pEffect.max_vel = vec3(.3f);
	pEffect.Generate();*/

	cGameObject* sphere = new cGameObject;
	sphere->meshName = "sphere_particle";
	sphere->shader.SetShader("particle");
	sphere->texture[0].SetBlend(1.f);

	quad.meshName = "screen_quad";
	quad.shader.SetShader("post");

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


	// Set up frame buffer
	cFBO* albedoFBO = new cFBO;
	cFBO* second_passFBO = new cFBO;
	cFBO* finalFBO = new cFBO;
	std::string fbo_error;
	if (albedoFBO->init(width, height, fbo_error))
	{
		printf("Frame buffer 1 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}
	if (second_passFBO->init(width, height, fbo_error))
	{
		printf("Frame buffer 2 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}
	if (finalFBO->init(width, height, fbo_error))
	{
		printf("Frame buffer 2 is OK\n");
	}
	else
	{
		printf("Frame buffer broke :(\n%s\n", fbo_error.c_str());
		exit(1);
	}

	// Run the start method on all behaviour components
	cBehaviourManager::Instance()->start();

	cGameObject* screen = cEntityManager::Instance()->GetGameObjectByTag("scope");
	cGameObject* player = cEntityManager::Instance()->GetGameObjectByTag("player");
	//cEntityManager::Instance()->GetGameObjectByTag("character")->refractivity = 1.0f;
	//cEntityManager::Instance()->GetGameObjectByTag("gun")->reflectivity = 0.2f;
	player->reflectivity = 1.0f;

	glEnable(GL_DEPTH);			// Write to the depth buffer
	glEnable(GL_DEPTH_TEST);	// Test with buffer when drawing

	while (!glfwWindowShouldClose(window))
	{
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

		int newWidth, newHeight;
		glfwGetFramebufferSize(window, &newWidth, &newHeight);
		float ratio = width / (float)height;

		if (newWidth != width || newHeight != height)
		{
			albedoFBO->reset(newWidth, newHeight, fbo_error);
			second_passFBO->reset(newWidth, newHeight, fbo_error);
			finalFBO->reset(newWidth, newHeight, fbo_error);
		}

		width = newWidth;
		height = newHeight;
		glViewport(0, 0, width, height);

		

		cBehaviourManager::Instance()->update(delta_time);
		cEntityManager::Instance()->Update(delta_time);

		g_PhysicsWorld->Update(delta_time);
		
		// Update 3D audio engine
		//scene->pAudioEngine->Update3d(scene->cameraEye, scene->cameraTarget, delta_time);

		// Move skybox relative to the camera
		pSkyBoxSphere->transform.pos = scene->camera.Eye;

		glm::mat4 p, v;

		// Projection matrix
		p = glm::perspective(0.6f,			// FOV
							 ratio,			// Aspect ratio
							 0.1f,			// Near clipping plane
							 1000.f);		// Far clipping plane

		vec3 dir = glm::normalize(vec3(0, 2.f, 0.f) - scene->camera.Eye);

		// View matrix
		v = glm::lookAt(vec3(0, 2.f, 0.f),
						vec3(0.f, 2.f, 0.f) + dir,
						scene->camera.Up);

		RenderObjectsToFBO(albedoFBO, width, height, p, v, delta_time);

		// set TV screen texture
		screen->texture[0].SetTexture(albedoFBO->colourTexture_ID);

		v = glm::lookAt(scene->camera.Eye,
			scene->camera.Target,
			scene->camera.Up);

		RenderObjectsToFBO(second_passFBO, width, height, p, v, delta_time);

		RenderQuadToFBO(*finalFBO, *second_passFBO);
		RenderQuadToScreen(*finalFBO);

		//DrawOctree(ship, phys->tree->main_node, bounds, ratio, v, p);
		renderer->RenderDebugObjects(v, p, 0.01f);
		

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);


	// Delete everything
	delete albedoFBO;
	delete second_passFBO;
	delete finalFBO;
	delete scene;
	ReleasePhysics();

	glfwTerminate();

	exit(EXIT_SUCCESS);
}


void SetUpTextureBindingsForObject(cGameObject* pCurrentObject)
{
	if (pass_id != 1) return;

	Shader& shader = pCurrentObject->shader;
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


void DrawObject(cGameObject* objPtr, glm::mat4 const& v, glm::mat4 const& p)
{
	Shader& shader = objPtr->shader;
	GLint shaderProgID = shader.GetID();

	GLint scopeUL = shader["isScope"];
	if (objPtr->tag == "scope")
	{
		glUniform1i(scopeUL, (int)GL_TRUE);
	}
	else
	{
		glUniform1i(scopeUL, (int)GL_FALSE);
	}



	GLint bIsSkyBox_UL = shader["isSkybox"];
	glUniform1f(shader["reflectivity"], objPtr->reflectivity);
	glUniform1f(shader["refractivity"], objPtr->refractivity);


	if (objPtr->tag != "skybox")
	{
		//if (objPtr->tag == "reflect")
		//{
		//	glCullFace(GL_BACK);
		//	glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);

		//	GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
		//	glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		//	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		//	// Tie the texture units to the samplers in the shader
		//	GLint skyBoxSampler_UL = shader["skyBox"];
		//	glUniform1i(skyBoxSampler_UL, 26);

		//}
		//else if (objPtr->tag == "refract")
		//{
		//	glCullFace(GL_BACK);
		//	glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);

		//	GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
		//	glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		//	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

		//	// Tie the texture units to the samplers in the shader
		//	GLint skyBoxSampler_UL = shader["skyBox"];
		//	glUniform1i(skyBoxSampler_UL, 26);

		//}
		//else
		{
			// Don't draw back facing triangles (default)
			glCullFace(GL_BACK);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			
			GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
			glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

			// Tie the texture units to the samplers in the shader
			GLint skyBoxSampler_UL = shader["skyBox"];
			glUniform1i(skyBoxSampler_UL, 26);


			SetUpTextureBindingsForObject(objPtr);
		}
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
		GLint skyBoxSampler_UL = shader["skyBox"];
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

	GLint matModelIT_UL = shader["matModelInverTrans"];
	glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	// ******* SCALE TRANSFORM *********
	/*glm::mat4 scale = glm::scale(glm::mat4(1.0f),
								 glm::vec3(objPtr->scale,
										   objPtr->scale,
										   objPtr->scale));
	m = m * scale;*/
	// ******* SCALE TRANSFORM *********
	

	

	GLint matModel_UL = shader["matModel"];
	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(objPtr->transform.ModelMatrix()));
	

	

	GLint eye_loc = shader["eyeLocation"];
	glUniform4f(eye_loc,
				scene->camera.Eye.x,
				scene->camera.Eye.y,
				scene->camera.Eye.z,
				1.0f
	);

	GLint diff_loc = shader["diffuseColour"];
	glUniform4f(diff_loc,
				objPtr->colour.x,
				objPtr->colour.y,
				objPtr->colour.z,
				objPtr->colour.w
	);

	GLint spec_loc = shader["specularColour"];
	glUniform4f(spec_loc,
				objPtr->specColour.x,
				objPtr->specColour.y,
				objPtr->specColour.z,
				objPtr->specIntensity
	);

	GLint isUniform_location = shader["isUniform"];
	glUniform1i(isUniform_location, objPtr->uniformColour);


	if (pass_id != 1)
	{
		scene->pLightManager->Set_Light_Data(shader);
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
		glUniform1i(obj->shader["isWater"], (int)GL_FALSE);

		objPtr->transform.pos = (node->AABB->min + (node->AABB->min + node->AABB->length)) / 2.f;
		objPtr->transform.scale = glm::vec3(node->AABB->length / 2.f);

		DrawObject(objPtr, v, p);
	}

	for (int i = 0; i < 8; ++i)
		DrawOctree(obj, node->nodes[i], objPtr, v, p);

}

void RenderObjectsToFBO(cSimpleFBO* fbo, float width, float height, glm::mat4 p, glm::mat4 v, float dt)
{
	/*if (!fbo->reset(width, height, errorString))
	{
		std::cout << "fbo was unable to be reset..." << std::endl;
		exit(1);
	}*/

	// Draw to the frame buffer
	fbo->use();
	fbo->clear_all();

	// Clear both the colour buffer (what we see) and the 
		//  depth (or z) buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_BLEND);      // Enable blend or "alpha" transparency
	//glDisable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//float ratio = width / height;


	int lastShader = -1;

	auto& objects = cEntityManager::Instance()->GetEntities();

	// Loop to draw everything in the scene
	for (int index = 0; index != objects.size(); index++)
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

		cGameObject* objPtr = objects[index];
		Shader& shader = objects[index]->shader;

		cMaterial* material = objPtr->GetComponent<cMaterial>();
		if (material != nullptr)
		{
			// TODO
		}

		objPtr->cmd_group->Update(dt);
		objPtr->brain->Update(dt);

		GLint shaderProgID = shader.GetID();

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

		DrawObject(objPtr, v, p);

	}//for (int index...
	// **************************************************

	// RENDER SKYBOX
	cGameObject* objPtr = pSkyBoxSphere;
	Shader& shader = objPtr->shader;

	cMaterial* material = objPtr->GetComponent<cMaterial>();
	if (material != nullptr)
	{
		// TODO
	}

	objPtr->cmd_group->Update(dt);
	objPtr->brain->Update(dt);

	GLint shaderProgID = shader.GetID();

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

	DrawObject(objPtr, v, p);

}
void RenderQuadToFBO(cFBO& fbo, cFBO& previousFBO)
{
	float width = previousFBO.width;
	float height = previousFBO.height;

	// 1. Disable the FBO

	// Draw to the frame buffer
	fbo.use();
	fbo.clear_all();

	// 2. Clear the ACTUAL screen buffer
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 3. Use the FBO colour texture as the texture on that quad
	GLint shaderProgID = quad.shader.GetID();
	Shader& shader = quad.shader;
	glUseProgram(shaderProgID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, previousFBO.colourTexture_ID);
	glUniform1i(shader["textSamp00"], 0);	// Texture unit 0

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, previousFBO.normalTexture_ID);
	glUniform1i(shader["textSamp01"], 1);	// Texture unit 1

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, previousFBO.positionTexture_ID);
	glUniform1i(shader["textSamp02"], 2);	// Texture unit 2

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, previousFBO.bloomTexture_ID);
	glUniform1i(shader["textSamp03"], 3);	// Texture unit 3

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, previousFBO.unlitTexture_ID);
	glUniform1i(shader["textSamp04"], 4);	// Texture unit 4

	// 4. Draw a single object (a triangle or quad)
	glUniform1i(shader["isFinalPass"], (int)GL_FALSE);

	glUniform2f(shader["iResolution"],
		width,
		height);

	glm::mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
	pass_id = 2;

	glUniformMatrix4fv(quad.shader["matProj"], 1, GL_FALSE, glm::value_ptr(p));

	DrawObject(&quad, glm::mat4(1.f), p);
}

void RenderQuadToScreen(cFBO& previousFBO)
{
	float width = previousFBO.width;
	float height = previousFBO.height;

	// LAST RENDER PASS
	GLint shaderProgID = quad.shader.GetID();
	Shader& shader = quad.shader;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, previousFBO.colourTexture_ID);
	glUniform1i(shader["textSamp00"], 0);	// Texture unit 0

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, previousFBO.bloomTexture_ID);
	glUniform1i(shader["textSamp03"], 3);	// Texture unit 3

	glUniform1i(shader["isFinalPass"], (int)GL_TRUE);

	glm::mat4 p = glm::ortho(-1.f, 1.f, -1.f, 1.f, -0.f, 1.f);
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

	DrawObject(&quad, glm::mat4(1.f), p);
}