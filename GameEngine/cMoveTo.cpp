#include "cMoveTo.h"
#include "cGameObject.h"
#include "Mathf.h"

cMoveTo::cMoveTo(cGameObject* instance, glm::vec3 position, float time)
{
	subject = instance;
	original_pos = subject->pos;
	target_pos = position;
	max_time = time;
	time_passed = 0.f;
}

void cMoveTo::Update(float delta_time)
{
	time_passed += delta_time;
	//float y = Mathf::smoothstep(0.f, 1.f, (time_passed / max_time));
	subject->pos = Mathf::autosmooth(original_pos, target_pos, (time_passed / max_time));
}

bool cMoveTo::Is_Done()
{
	return (time_passed >= max_time);
}
