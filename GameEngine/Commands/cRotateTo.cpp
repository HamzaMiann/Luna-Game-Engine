#include "cRotateTo.h"
#include <cGameObject.h>
#include <Physics/Mathf.h>


cRotateTo::cRotateTo(cGameObject* instance, glm::vec3 angle, float time, float ease_time)
{
	subject = instance;
	start = subject->transform.rotation;
	angle.x = glm::radians(angle.x);
	angle.y = glm::radians(angle.y);
	angle.z = glm::radians(angle.z);
	end = subject->transform.rotation * glm::quat(angle);
	this->ease_time = ease_time;
	max_time = time + ease_time;
	time_passed = 0.f;
}

void cRotateTo::Update(float delta_time)
{
	if (time_passed == 0.f) start = subject->transform.rotation;
	time_passed += delta_time;
	float ratio = time_passed / max_time;
	if (ease_time != 0.f) ratio = Mathf::smootherstep(0.f, 1.f, ratio);

	glm::quat rot = glm::slerp(start, end, ratio);
	subject->transform.rotation = rot;
}

bool cRotateTo::Is_Done()
{
	return (time_passed >= max_time);
}
