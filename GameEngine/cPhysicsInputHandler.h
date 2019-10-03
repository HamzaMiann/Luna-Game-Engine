#pragma once

#include "iInputHandler.h"

class Scene;

class cPhysicsInputHandler : public iInputHandler
{
private:
	Scene& _scene;
public:
	cPhysicsInputHandler() = delete;
	cPhysicsInputHandler(Scene& scene) : _scene(scene) {}
	virtual ~cPhysicsInputHandler() {}
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};