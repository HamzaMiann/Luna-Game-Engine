#pragma once


#include "iCommand.h"
#include <glm/vec3.hpp>

class cGameObject;

class cFollowCurve : public iCommand
{
	cGameObject* subject;

	glm::vec3 original_pos;
	glm::vec3 target_pos;
	glm::vec3 offset;

	float max_time;
	float time_passed;
	float ease_time;

public:
	cFollowCurve(cGameObject* instance, glm::vec3 target, glm::vec3 offset, float time, float ease_time);
	virtual ~cFollowCurve() {}
	virtual void Update(float delta_time);
	virtual bool Is_Done();
};