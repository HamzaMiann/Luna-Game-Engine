#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <iObject.h>
#include <Commands/cCommandGroup.h>
#include <Texture/cTexture.h>
#include <Shader/Shader.h>


class cSimpleAssimpSkinnedMesh;
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
	Shader* shader;

	cSimpleAssimpSkinnedMesh* animation = 0;

	cLuaBrain* brain;
	std::string lua_script;

	std::string meshName = "";

	glm::vec4  colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

	float reflectivity = 0.f;
	float refractivity = 0.f;

	glm::vec3 specColour = glm::vec3(.5f, .5f, .5f);
	float specIntensity = 1.f;
	bool uniformColour = false;
	bool isWireframe = false;

	ColliderType Collider = ColliderType::NONE;
	std::vector<glm::vec3> CollidePoints;
	bool isCollided = false;

	cTexture texture[4];

	cGameObject* parent;
	std::vector<cGameObject*> children;

	cCommandGroup* cmd_group;


	// Inherited via iObject
	virtual std::string ObjectName() override;

};

#endif


