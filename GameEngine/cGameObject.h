#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>

class Scene;

enum ColliderType
{
	SPHERE,
	CUBE,
	MESH,
	AABB,
	NONE
};

class cGameObject
{
public:
	cGameObject()
	{
		this->textureRatio[0] = 1.f;
		this->textureRatio[1] = 0.f;
		this->textureRatio[2] = 0.f;
		this->textureRatio[3] = 0.f;
		this->texture[0] = "pebbles-beach-textures.bmp";
	}
	virtual ~cGameObject() {}

	std::string meshName = "";
	std::string tag = "";
	glm::vec3  pos = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3  previousPos = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3  rotation = glm::vec3(0.f, 0.f, 0.f);
	glm::vec4  colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	float scale = 1.f;

	glm::vec3 specColour = glm::vec3(.5f, .5f, .5f);
	float specIntensity = 1.f;
	bool customSpecularity = false;
	bool uniformColour = false;
	bool isWireframe = false;

	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 acceleration = glm::vec3(0.f, 0.f, 0.f);
	float gravityScale = 0.f;
	float inverseMass = 1.f;

	ColliderType Collider = ColliderType::NONE;
	bool isCollided = false;

	std::string texture[4];
	float textureRatio[4];

	std::vector<cGameObject*> children;

	virtual void UpdateLogic(Scene* scene) {}
};

#endif


