#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <string>
#include <Camera.h>
#include <iObject.h>
#include <Components/cMaterial.h>

class cGameObject;
class cAABB;

class RenderingEngine
{
private:
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;
	typedef glm::mat4 mat4;

	RenderingEngine();

	mat4 projection;
	mat4 view;

	int height;
	int width;

	int pass_id;

public:

	Camera camera;

	~RenderingEngine();

	static RenderingEngine* Instance()
	{
		static RenderingEngine instance;
		return &instance;
	}

	Camera* MainCamera();

	void UpdateView();
	void SetUpTextureBindingsForObject(cMaterial* material, int shaderProgID);
	void Render(iObject* object);

};