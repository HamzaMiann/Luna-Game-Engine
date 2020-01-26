#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>
#include <Commands/cCommandGroup.h>
#include <iObject.h>

class Scene;
class cLuaBrain;

enum ColliderType
{
	SPHERE,
	CUBE,
	MESH,
	POINT,
	AABB,
	NONE
};

class cGameObject : public iObject
{
public:

	cGameObject();
	
	virtual ~cGameObject();

	unsigned int id;
	cLuaBrain* brain;
	std::string lua_script;

	std::string meshName = "";
	std::string tag = "";

	std::string shaderName = "basic";
	glm::vec4  colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

	glm::vec3 specColour = glm::vec3(.5f, .5f, .5f);
	float specIntensity = 1.f;
	bool uniformColour = false;
	bool isWireframe = false;

	ColliderType Collider = ColliderType::NONE;
	std::vector<glm::vec3> CollidePoints;
	bool isCollided = false;

	std::string texture[4];
	float textureRatio[4];

	std::vector<cGameObject*> children;

	glm::mat4 ModelMatrix();
	glm::mat4 TranslationMatrix();
	glm::mat4 RotationMatrix();
	glm::mat4 ScaleMatrix();

	cCommandGroup* cmd_group;


	// Inherited via iObject
	virtual std::string ObjectName() override;

};

#endif


