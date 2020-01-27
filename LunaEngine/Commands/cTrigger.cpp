#include "cTrigger.h"
#include "cGameObject.h"

cTrigger::cTrigger(cGameObject * instance, glm::vec3 target, float distance)
{
	this->subject = instance;
	this->target_pos = target;
	this->distance = distance;
}

void cTrigger::Update(float delta_time)
{
}

bool cTrigger::Is_Done()
{
	return glm::distance(subject->transform.pos, target_pos) < distance;
}
