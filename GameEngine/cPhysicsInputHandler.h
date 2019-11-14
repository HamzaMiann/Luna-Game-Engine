#pragma once

#include "iInputHandler.h"

class Scene;

class cPhysicsInputHandler : public iInputHandler
{
private:
	double previousX = 0.0, previousY = 0.0;
	Scene& _scene;
public:
	cPhysicsInputHandler() = delete;
	cPhysicsInputHandler(Scene& scene) : _scene(scene) {}
	virtual ~cPhysicsInputHandler() {}
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};