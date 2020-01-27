#include "cMoveTo.h"
#include <cGameObject.h>
#include <Physics/Mathf.h>

cMoveTo::cMoveTo(cGameObject* instance, glm::vec3 position, float time, float ease_time)
{
	subject = instance;

	original_pos = subject->transform.pos;
	target_pos = position;

	max_time = time + ease_time * 2.f;

	this->ease_begin_time = ease_time;
	this->ease_end_time = max_time - ease_time;

	this->ease_begin_target = ((ease_time / max_time) * (target_pos - original_pos)) + original_pos;
	this->ease_end_target = target_pos + ((ease_time / max_time) * (original_pos - target_pos));

	time_passed = 0.f;
}

void cMoveTo::Update(float delta_time)
{
	if (time_passed == 0.f) original_pos = subject->transform.pos;
	time_passed += delta_time;
	if (time_passed > max_time) time_passed = max_time;

	float ratio = (time_passed / max_time);
	if (ease_begin_time != 0.f) ratio = Mathf::smootherstep(0.f, 1.f, ratio);
	subject->transform.pos = Mathf::lerp(original_pos, target_pos, ratio);
}

bool cMoveTo::Is_Done()
{
	return (time_passed >= max_time);
}
