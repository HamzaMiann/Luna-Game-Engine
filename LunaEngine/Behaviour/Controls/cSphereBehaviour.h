#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <iObject.h>
#include <interfaces/physics/iPhysicsComponent.h>

class cSphereBehaviour : public iBehaviour
{
public:

	cSphereBehaviour(iObject* root);
	virtual ~cSphereBehaviour();

	virtual void start() override;

	virtual void update(float dt) override;


private:
	nPhysics::iPhysicsComponent* body;

	glm::quat rotX;
	glm::quat rotY;
	static glm::vec3 direction;

	glm::vec3 camera_target;
	glm::vec3 camera_position;

	static float distance_from_object;

	float previousX = 0.f;
	float previousY = 0.f;

};