#pragma once

#include <vector>
#include <string>
#include "GLCommon.h"
#include <glm/vec3.hpp>
//#include "cGameObject.h"
//#include "AudioEngine.hpp"
//#include "cVAOManager.h"
//#include "cModelLoader.h"
//#include "cLightManager.h"

class cGameObject;
class cVAOManager;
class cModelLoader;
class AudioEngine;
class cLightManager;

class Scene
{
private:
	typedef std::vector<cGameObject*> object_list;
public:
	Scene() {}
	~Scene();

	static Scene* LoadFromXML(std::string filename);

	std::string SceneName;

	int shaderProgID = -1;

	object_list vecGameObjects;
	cVAOManager* pVAOManager;
	cModelLoader* pModelLoader;
	AudioEngine* pAudioEngine;
	cLightManager* pLightManager;

	glm::vec3 cameraEye = glm::vec3(0.0, 1.0f, -10.0);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightLocation = glm::vec3(0.0f, 0.0f, 0.0f);

};