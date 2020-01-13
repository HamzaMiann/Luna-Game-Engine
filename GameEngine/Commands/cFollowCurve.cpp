
#include "cFollowCurve.h"
#include <cGameObject.h>
#include <DebugRenderer/cDebugRenderer.h>
#include <Physics/Mathf.h>

#define PDEBUG

typedef glm::vec3 vec3;

cFollowCurve::cFollowCurve(cGameObject* instance, glm::vec3 target, glm::vec3 offset, float time, float ease_time)
{
	subject = instance;

	target_pos = target;
	original_pos = subject->transform.pos;

	this->offset = offset;

	max_time = time + ease_time;
	this->ease_time = ease_time;

	time_passed = 0.f;


}

void cFollowCurve::Update(float delta_time)
{
	if (time_passed == 0.f)
	{
		original_pos = subject->transform.pos;
#ifdef PDEBUG
		vec3 last_pos = original_pos;
		for (float i = 0.f; i < max_time; i += 0.1f)
		{
			float ratio = i / max_time;
			float inverse = (max_time - i) / max_time;

			vec3 pointA = ((offset - original_pos) * ratio) + original_pos;
			vec3 pointB = ((offset - target_pos) * inverse) + target_pos;

			vec3 line = pointB - pointA;
			vec3 pos = (line * ratio) + pointA;

			cDebugRenderer::Instance()->addLine(last_pos, pos, vec3(1.f, 0.f, 0.f), max_time);

			last_pos = pos;
		}
#endif
	}
	time_passed += delta_time;
	if (time_passed > max_time) time_passed = max_time;

	float ratio = time_passed / max_time;
	if (ease_time != 0.f) ratio = Mathf::smootherstep(0.f, 1.f, ratio);
	float inverse = 1.f - ratio;

	vec3 pointA = ((offset - original_pos) * ratio) + original_pos;
	vec3 pointB = ((offset - target_pos) * inverse) + target_pos;

	vec3 line = pointB - pointA;
	vec3 pos = (line * ratio) + pointA;

	subject->transform.pos = pos;
}

bool cFollowCurve::Is_Done()
{
	return (time_passed >= max_time);
}
