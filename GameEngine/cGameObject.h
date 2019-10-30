#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>

class Scene;

enum ColliderType
{
	SPHERE,
	CUBE,
	MESH,
	NONE
};

class cGameObject
{
public:
	cGameObject() { }
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

	std::vector<cGameObject*> children;

	virtual void UpdateLogic(Scene* scene) {}
};

#endif


