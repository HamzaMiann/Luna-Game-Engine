#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <Camera.h>

class cGameObject;
class cAABB;

class RenderingEngine
{
private:
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	RenderingEngine() {}

public:

	Camera camera;

	~RenderingEngine() {}

	static RenderingEngine* Instance()
	{
		static RenderingEngine instance;
		return &instance;
	}


};