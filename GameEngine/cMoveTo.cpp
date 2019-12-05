#include "cMoveTo.h"
#include "cGameObject.h"
#include "Mathf.h"

cMoveTo::cMoveTo(cGameObject* instance, glm::vec3 position, float time, float ease_time)
{
	subject = instance;

	original_pos = subject->pos;
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
	time_passed += delta_time;
	if (time_passed > max_time) time_passed = max_time;

	if (time_passed < ease_begin_time)
	{
		subject->pos = Mathf::autosmooth(original_pos,
										 ((ease_begin_target - original_pos) * 2.f) + original_pos,
										 (time_passed / ease_begin_time) / 2.f);
	}
	else if (time_passed > ease_end_time)
	{
		//float x = 1.0 - Mathf::smoothstep(0.f, 1.f, ((time_passed - ease_end_time) / ease_begin_time));

		subject->pos = Mathf::autosmooth(ease_end_target + (ease_end_target - target_pos),
										 target_pos,
										 ((time_passed - ease_end_time) / ease_begin_time) / 2.f) + 0.5f;
		printf("%f\n", (float)(((time_passed - ease_end_time) / ease_begin_time) / 2.f) + 0.5f);
		//subject->pos = ((ease_end_target - target_pos) * x) + target_pos;
		//subject->pos = Mathf::autosmooth(ease_end_target, target_pos, (((time_passed - ease_end_time) / ease_begin_time)));
	}
	else
	{
		subject->pos = Mathf::lerp(ease_begin_target, ease_end_target, ((time_passed - ease_begin_time) / (max_time - 2.f * ease_begin_time)));
	}
	//float y = Mathf::smoothstep(0.f, 1.f, (time_passed / max_time));
	//subject->pos = Mathf::autosmooth(original_pos, target_pos, (time_passed / max_time));
}

bool cMoveTo::Is_Done()
{
	return (time_passed >= max_time);
}
