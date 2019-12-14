#pragma once


#include "iCommand.h"
#include <glm/vec3.hpp>

class cGameObject;

class cTrigger : public iCommand
{
	cGameObject* subject;

	glm::vec3 target_pos;
	float distance;

public:
	cTrigger(cGameObject* instance, glm::vec3 target, float distance);
	virtual ~cTrigger() {}
	virtual void Update(float delta_time);
	virtual bool Is_Done();
};