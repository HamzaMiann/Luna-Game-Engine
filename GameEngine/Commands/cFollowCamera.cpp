#include "cFollowCamera.h"
#include "cGameObject.h"

typedef glm::vec3 vec3;

cFollowCamera::cFollowCamera(cGameObject* instance, float min_dist, glm::vec3 offset, float time, Camera* camera)
{
	subject = instance;
	this->min_distance = min_dist;
	this->offset = offset;
	this->cam = camera;
	this->elapsed_time = 0.f;
	this->max_time = time;
}

void cFollowCamera::Update(float delta_time)
{
	elapsed_time += delta_time;

	cam->Target = subject->transform.pos + offset;
	if (glm::distance(cam->Eye, cam->Target) > min_distance)
	{
		vec3 direction = cam->Target - cam->Eye;
		cam->Eye += direction * delta_time;
	}
}

bool cFollowCamera::Is_Done()
{
	return elapsed_time > max_time;
}
