#pragma once

#include "iCommand.h"
#include "Camera.h"

class cGameObject;

class cFollowCamera : public iCommand
{
	cGameObject* subject;

	float min_distance;
	glm::vec3 offset;
	Camera* cam;

	float max_time;
	float elapsed_time;

public:
	cFollowCamera(cGameObject* instance, float min_dist, glm::vec3 offset, float time, Camera* camera);
	virtual ~cFollowCamera() {}
	virtual void Update(float delta_time);
	virtual bool Is_Done();
};