#pragma once

#include "iInputHandler.h"

class Scene;

class cAudioInputHandler : public iInputHandler
{
private:
	Scene& _scene;
public:
	cAudioInputHandler() = delete;
	cAudioInputHandler(Scene& scene) : _scene(scene) {}
	virtual ~cAudioInputHandler() {}
	virtual void HandleInput(GLFWwindow* window);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};
