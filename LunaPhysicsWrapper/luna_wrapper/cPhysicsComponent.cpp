#include "cPhysicsComponent.h"
#include <luna_physics/shapes.h>

cSphereComponent::cSphereComponent(iObject* parent, const nPhysics::sSphereDef& definition)
	: iSphereComponent(parent)
	, phys::cRigidBody(phys::sRigidBodyDef(), new phys::cSphere(definition.Position, definition.Radius))
{
}

void cSphereComponent::GetTransform(glm::mat4& transformOut)
{
}

bool cSphereComponent::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cSphereComponent::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

cPlaneComponent::cPlaneComponent(iObject* parent, const nPhysics::sPlaneDef& definition)
	: iPlaneComponent(parent)
	, phys::cRigidBody(phys::sRigidBodyDef(), new phys::cPlane(definition.Normal, definition.Constant))
{
}

void cPlaneComponent::GetTransform(glm::mat4& transformOut)
{
}

bool cPlaneComponent::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cPlaneComponent::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}
