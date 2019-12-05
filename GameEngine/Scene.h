#pragma once

#include <vector>
#include <string>
#include "GLCommon.h"
#include "Camera.h"
#include <map>

class cGameObject;
class cVAOManager;
class cModelLoader;
class AudioEngine;
class cLightManager;

class Scene
{
private:
	typedef			std::vector<cGameObject*>	object_list;
	typedef			glm::vec3					vec3;
public:
	Scene() {}
	~Scene();

	static Scene*	LoadFromXML(std::string filename);

	std::string		SceneName;

	std::map<std::string, unsigned int>	Shaders;

	object_list		vecGameObjects;
	cVAOManager*	pVAOManager;
	cModelLoader*	pModelLoader;
	AudioEngine*	pAudioEngine;
	cLightManager*	pLightManager;

	Camera camera;


	bool SaveToFile();
	bool SaveLights();
	bool SaveLayout();
	bool SaveAudio();
};