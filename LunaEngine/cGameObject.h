#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <iObject.h>
#include <Commands/cCommandGroup.h>
#include <Texture/cTexture.h>



class cSimpleAssimpSkinnedMesh;
class cLuaBrain;
class Shader;

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

	cLuaBrain* brain;
	std::string lua_script;

	std::string meshName;

	glm::vec4  colour = glm::vec4(1.f, 1.f, 1.f, 1.f);

	float reflectivity = 0.f;
	float refractivity = 0.f;

	glm::vec3 specColour;
	float specIntensity;
	bool uniformColour = false;
	bool isWireframe = false;

	ColliderType Collider = ColliderType::NONE;
	std::vector<glm::vec3> CollidePoints;
	bool isCollided = false;

	cTexture texture[4];
	cCommandGroup* cmd_group;

	bool shouldBlend;
	cTexture blendMap;


	// Inherited via iObject
	virtual std::string ObjectName() override;

};

#endif


