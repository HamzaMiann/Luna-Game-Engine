
#include <cGameObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Lua/cLuaBrain.h>

static unsigned int next_id = 0;

cGameObject::cGameObject()
{
	id = next_id++;
	brain = new cLuaBrain(this);
	shader = 0;
	parent = 0;
	cmd_group = new cCommandGroup;
	shouldBlend = false;
	specColour = glm::vec3(.0f, .0f, .0f);
	specIntensity = 100.f;
	meshName = "";
}

cGameObject::~cGameObject()
{
	delete brain;
	brain = 0;
}
std::string cGameObject::ObjectName()
{
	return "cGameObject";
}
