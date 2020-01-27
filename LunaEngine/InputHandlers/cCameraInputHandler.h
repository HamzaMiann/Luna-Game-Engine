#pragma once

#include "iInputHandler.h"

class Scene;
class GLFWwindow;
class cGameObject;

class cCameraInputHandler : public iInputHandler
{
private:
	double previousX = 0.0, previousY = 0.0;
	Scene& _scene;
	GLFWwindow* _window;
	cGameObject* player;

public:
	cCameraInputHandler() = delete;
	cCameraInputHandler(Scene& scene, GLFWwindow* window);
	virtual ~cCameraInputHandler();
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};