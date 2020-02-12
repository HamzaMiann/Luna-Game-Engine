
#include <cGameObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Lua/cLuaBrain.h>

static unsigned int next_id = 0;

cGameObject::cGameObject()
{
	id = next_id++;
	brain = new cLuaBrain(this);

	cmd_group = new cCommandGroup;
}

cGameObject::~cGameObject()
{
	delete brain;
}
std::string cGameObject::ObjectName()
{
	return "cGameObject";
}
