#pragma once

#include <vector>
#include <string>
#include <_GL/GLCommon.h>
#include "Camera.h"
#include <map>

//class cGameObject;

class Scene
{
public:
	Scene() {}
	~Scene();

	static Scene*	LoadFromXML(std::string filename);

	std::string		SceneName;
	Camera camera;


	bool SaveToFile();
	bool SaveLights();
	bool SaveLayout();
	bool SaveAudio();
};