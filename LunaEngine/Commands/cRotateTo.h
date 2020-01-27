#pragma once

#include "iCommand.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class cGameObject;

class cRotateTo : public iCommand
{
	cGameObject* subject;
	glm::quat start;
	glm::quat end;
	float max_time;
	float time_passed;
	float ease_time;

public:
	cRotateTo(cGameObject* instance, glm::vec3 angle, float time, float ease_time);
	virtual ~cRotateTo() {}
	virtual void Update(float delta_time);
	virtual bool Is_Done();
};