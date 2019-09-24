#pragma once

#include "iInputHandler.h"

class cPhysicsInputHandler : public iInputHandler
{
public:
	virtual ~cPhysicsInputHandler() {}
	virtual void HandleInput(Scene& scene, GLFWwindow* window) {}
	virtual void key_callback(Scene& scene, GLFWwindow* window, int key, int scancode, int action, int mods) {}
};