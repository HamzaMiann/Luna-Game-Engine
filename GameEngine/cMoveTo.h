#pragma once


#include "iCommand.h"
#include <glm/vec3.hpp>

class cGameObject;

class cMoveTo : public iCommand
{
	cGameObject* subject;

	glm::vec3 target_pos;
	glm::vec3 original_pos;
	glm::vec3 ease_begin_target;
	glm::vec3 ease_end_target;

	float max_time;
	float time_passed;
	float ease_begin_time;
	float ease_end_time;

public:
	cMoveTo(cGameObject* instance, glm::vec3 position, float time, float ease_time);
	virtual ~cMoveTo() {}
	virtual void Update(float delta_time);
	virtual bool Is_Done();
};