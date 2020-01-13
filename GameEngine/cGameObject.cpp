
#include <cGameObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Lua/cLuaBrain.h>

static unsigned int next_id = 0;

cGameObject::cGameObject()
{
	id = next_id++;
	brain = new cLuaBrain(this);

	this->textureRatio[0] = 1.f;
	this->textureRatio[1] = 0.f;
	this->textureRatio[2] = 0.f;
	this->textureRatio[3] = 0.f;
	this->texture[0] = "";
	this->texture[1] = "";
	this->texture[2] = "";
	this->texture[3] = "";

	cmd_group = new cCommandGroup;
}

cGameObject::~cGameObject()
{
	delete brain;
}

glm::mat4 cGameObject::ModelMatrix()
{
	return ((glm::mat4(1.f) * TranslationMatrix()) * RotationMatrix()) * ScaleMatrix();
}

glm::mat4 cGameObject::TranslationMatrix()
{
	return glm::translate(glm::mat4(1.0f),
						  glm::vec3(transform.pos.x,
									transform.pos.y,
									transform.pos.z)
	);
}

glm::mat4 cGameObject::RotationMatrix()
{
	glm::mat4 m(1.f);
	m *= glm::mat4(transform.rotation);
	return m;
}

glm::mat4 cGameObject::ScaleMatrix()
{
	return glm::scale(glm::mat4(1.0f),
					  glm::vec3(transform.scale.x,
								transform.scale.y,
								transform.scale.z)
	);
}