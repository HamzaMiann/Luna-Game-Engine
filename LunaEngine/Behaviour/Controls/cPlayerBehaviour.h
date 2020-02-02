#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <iObject.h>
#include <interfaces/physics/iPhysicsComponent.h>

class cPlayerBehaviour : public iBehaviour
{
	typedef glm::vec3 vec3;
public:

	cPlayerBehaviour(iObject* root);
	virtual ~cPlayerBehaviour();

	// Inherited via iBehaviour
	virtual bool serialize(rapidxml::xml_node<>* root_node) override;

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

	virtual void start() override;

	virtual void update(float dt) override;

	virtual void OnCollide(iObject* other) override;

	void Reset();

	const vec3 forward = glm::vec3(0.f, 0.f, 1.f);
	vec3 direction;

private:
	nPhysics::iPhysicsComponent* rb;

	vec3 start_pos;
	float reload_time;

};