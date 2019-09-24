#pragma once

#include "GLCommon.h"
#include "Scene.h"

class iInputHandler
{
public:
	virtual ~iInputHandler() {}
	virtual void HandleInput(Scene& scene, GLFWwindow* window) = 0;
	virtual void key_callback(Scene& scene, GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
};