#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <iObject.h>

class cFPSController : public iBehaviour
{
	typedef glm::vec3 vec3;
	typedef glm::quat quat;
public:

	cFPSController(iObject* root) : iBehaviour(root) {}
	virtual ~cFPSController() {}

	// Inherited via iBehaviour
	virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }

	virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

	virtual void start() override;

	virtual void update(float dt) override;


private:

	quat rotX;
	float maxAngleY;
	float angleY;

	vec3 direction;
	vec3 position_offset;

	float speed;

	float previousX = 0.f;
	float previousY = 0.f;

};