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

	std::string meshName;
	glm::vec3  positionXYZ = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3  rotationXYZ = glm::vec3(0.f, 0.f, 0.f);
	glm::vec4  objectColourRGBA = glm::vec4(1.f, 1.f, 1.f, 1.f);
	float scale = 1.f;

	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 acceleration = glm::vec3(0.f, 0.f, 0.f);

	float inverseMass = 1.f;	// 0.0f = infinite mass (Doesn't move)

	ColliderType Collider = ColliderType::NONE;
	bool isCollided = false;

	virtual void UpdateLogic(Scene* scene) {}
};

#endif


