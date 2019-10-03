#pragma once

#include <vector>
#include "cGameObject.h"
#include "AudioEngine.hpp"
#include "cVAOManager.h"
#include "cModelLoader.h"

class Scene
{
private:
	typedef std::vector<cGameObject*> object_list;
public:
	Scene() {}
	~Scene();

	static Scene* LoadFromXML(std::string filename);

	std::string SceneName;

	GLint shaderProgID = -1;

	object_list vecGameObjects;
	cVAOManager* pVAOManager;
	cModelLoader* pModelLoader;
	AudioEngine* pAudioEngine;

	glm::vec3 cameraEye = glm::vec3(0.0, 1.0f, -10.0);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightLocation = glm::vec3(0.0f, 0.0f, 0.0f);

};