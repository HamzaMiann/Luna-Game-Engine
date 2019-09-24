#pragma once

#include <vector>
#include "cGameObject.h"
#include "AudioEngine.hpp"
#include "cVAOManager.h"

class Scene
{
public:

	static Scene* BuildFromXML(std::string filename);

	glm::vec3 cameraEye = glm::vec3(0.0, 1.0f, -7.0);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 LightLocation = glm::vec3(0.0f, 0.0f, 0.0f);

	std::vector<cGameObject*> vecGameObjects;
	cVAOManager* pVAOManager;
	AudioEngine* pAudioEngine;

};