#include "cRigidBody.h"
#include <Physics/PhysicsEngine.h>
#include <xml_helper.h>

cRigidBody::cRigidBody(iObject* obj)
	: transform(obj->transform)
{
	velocity = vec3(0.f);
	acceleration = vec3(0.f);
	force = vec3(0.f);
	previousPos = transform.pos;
	gravityScale = 0.f;
	inverseMass = 1.f;
	PhysicsEngine::Instance()->Register(this);
}

cRigidBody::~cRigidBody()
{
	PhysicsEngine::Instance()->Unregister(this);
}

bool cRigidBody::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cRigidBody::deserialize(rapidxml::xml_node<>* root_node)
{
	for (rapidxml::xml_node<>* node = root_node->first_node();
		 node; node = node->next_sibling())
	{
		std::string name = node->name();
		if (name == "Velocity")
		{
			this->velocity = XML_Helper::AsVec3(node);
		}
		else if (name == "Acceleration")
		{
			this->acceleration = XML_Helper::AsVec3(node);
		}
		else if (name == "Mass")
		{
			float mass = XML_Helper::AsFloat(node);
			if (mass == 0.f) this->inverseMass = 0.f;
			else this->inverseMass = 1.f / mass;
		}
		else if (name == "Gravity")
		{
			this->gravityScale = XML_Helper::AsFloat(node);
		}
	}
	return true;
}
