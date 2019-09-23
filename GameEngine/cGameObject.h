#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <string>

class cGameObject
{
public:
	cGameObject()
	{
		scale = 0.0f;
	}
	std::string meshName;			//"Pirate"
	glm::vec3  positionXYZ;
	glm::vec3  rotationXYZ;
	glm::vec4  objectColourRGBA;
	float scale;

	// Add physics stuff
	glm::vec3 velocity;
	glm::vec3 acceleration;

	// If the object has an inverse mass of 0.0
	// then it's not updated by the physics code
	float inverseMass;	// 0.0f = infinite mass (Doesn't move)

};

#endif


