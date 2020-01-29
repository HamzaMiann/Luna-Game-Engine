#pragma once

#include <Behaviour/iBehaviour.h>
#include <iObject.h>
class cRigidBody;

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
	cRigidBody* body;

	glm::quat rotation;
	glm::vec3 direction = glm::vec3(0.f, 0.f, 1.f);

	glm::vec3 camera_target;
	glm::vec3 camera_position;

	float distance_from_object = 2.f;

	float previousX = 0.f;
	float previousY = 0.f;

};