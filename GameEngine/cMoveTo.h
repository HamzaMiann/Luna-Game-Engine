#pragma once


#include "iCommand.h"
#include <glm/vec3.hpp>

class cGameObject;

class cMoveTo : public iCommand
{
	cGameObject* subject;
	glm::vec3 target_pos;
	glm::vec3 original_pos;
	float max_time;
	float time_passed;

public:
	cMoveTo(cGameObject* instance, glm::vec3 position, float time);
	virtual ~cMoveTo() {}
	virtual void Update(float delta_time);
	virtual bool Is_Done();
};