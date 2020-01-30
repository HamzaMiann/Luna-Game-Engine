#pragma once

#include <Behaviour/iBehaviour.h>
#include <iObject.h>
#include <interfaces/physics/iPhysicsComponent.h>

class cSphereBehaviour : public iBehaviour
{
public:

	cSphereBehaviour(iObject* root);
	virtual ~cSphereBehaviour();

	// Inherited via iBehaviour
	virtual bool serialize(rapidxml::xml_node<>* root_node) override;

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

	virtual void start() override;

	virtual void update(float dt) override;

private:
	nPhysics::iPhysicsComponent* body;

	glm::quat rotation;
	static glm::vec3 direction;

	glm::vec3 camera_target;
	glm::vec3 camera_position;

	static float distance_from_object;

	float previousX = 0.f;
	float previousY = 0.f;

};