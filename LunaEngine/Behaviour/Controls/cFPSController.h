#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <iObject.h>
#include <Components\cAnimationController.h>
#include <interfaces\physics\iPhysicsComponent.h>

class cFPSController : public iBehaviour
{
	typedef glm::vec3 vec3;
	typedef glm::quat quat;
public:

	cFPSController(iObject* root) : iBehaviour(root) {}
	virtual ~cFPSController() {}

	// Inherited via iBehaviour
	virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

	virtual void start() override;

	virtual void update(float dt) override;

	virtual void OnDestroy() override;


private:

	nPhysics::iPhysicsComponent* rigidBody;
	iObject* weapon;

	vec3 direction;
	vec3 jumpVelocity;
	float previousX = 0.f, previousY = 0.f;

	quat rotX;
	quat rotY;

	float animationDuration = 0.f;

	struct Settings
	{
		vec3 forward = glm::vec3(0.f, 0.f, 1.f);
		vec3 camera_offset = vec3(0.f);

		float mouse_speed = 0.1f;
		float distance_from_object = 2.f;
		float speed = 1.f;
	} settings;

};