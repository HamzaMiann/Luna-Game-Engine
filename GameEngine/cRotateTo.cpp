#include "cRotateTo.h"
#include "cGameObject.h"
#include "Mathf.h"

cRotateTo::cRotateTo(cGameObject* instance, glm::vec3 angle, float time)
{
	subject = instance;
	start = subject->getQOrientation();
	angle.x = glm::radians(angle.x);
	angle.y = glm::radians(angle.y);
	angle.z = glm::radians(angle.z);
	end = subject->getQOrientation() * glm::quat(angle);
	max_time = time;
	time_passed = 0.f;
}

void cRotateTo::Update(float delta_time)
{
	time_passed += delta_time;
	glm::quat rot = glm::slerp(start, end, (time_passed / max_time));
	subject->setOrientation(rot);
}

bool cRotateTo::Is_Done()
{
	return (time_passed >= max_time);
}
