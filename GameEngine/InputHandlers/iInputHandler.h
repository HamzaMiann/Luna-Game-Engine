#pragma once

#include <_GL/GLCommon.h>

class iInputHandler
{
public:
	virtual ~iInputHandler() {}
	virtual void HandleInput(GLFWwindow* window) = 0;
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
};