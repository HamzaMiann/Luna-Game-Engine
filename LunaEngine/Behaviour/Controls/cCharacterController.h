#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <interfaces/physics/iPhysicsComponent.h>
#include <Components/cAnimationController.h>
#include <glm/glm_common.h>

struct sCharacterControllerSettings
{
	vec3 forward = glm::vec3(0.f, 0.f, 1.f);
	vec3 camera_offset = vec3(0.f);

	float mouse_speed = 0.1f;
	float distance_from_object = 2.f;
	float speed = 1.f;
};

class cCharacterController : public iBehaviour
{
public:

	cCharacterController(iObject* parent)
		: iBehaviour(parent)
	{ }

	virtual ~cCharacterController() {}

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

	virtual void start() override;

	virtual void update(float dt) override;

	virtual void OnDestroy() override;

	inline sCharacterControllerSettings GetSettings() { return settings; }

private:

	nPhysics::iPhysicsComponent* rigidBody;
	cAnimationController* anim;

	vec3 direction;
	float previousX = 0.f, previousY = 0.f;

	quat rotX;
	quat rotY;

	bool isJumping = false;
	float jumpDuration = 0.f;

	sCharacterControllerSettings settings;

};