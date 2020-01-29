#include "cSphereComponent.h"
#include <luna_physics/shapes.h>
#include <xml_helper.h>

cSphereComponent::cSphereComponent(iObject* parent, const phys::sRigidBodyDef& definition)
	: cSphereBody(definition)
	, iSphereComponent(parent)
{
}

cSphereComponent::cSphereComponent(iObject* parent, const phys::sRigidBodyDef& bodyDef, const nPhysics::sSphereDef& sphereDef)
	: cSphereBody(bodyDef, sphereDef.Position, sphereDef.Radius)
	, iSphereComponent(parent)
{
}

const glm::vec3& cSphereComponent::GetPosition()
{
	return this->mPosition;
}

void cSphereComponent::SetPosition(const glm::vec3& pos)
{
	this->mPosition = pos;
	this->UpdateTransform();
}

const glm::vec3& cSphereComponent::GetVelocity()
{
	return this->mVelocity;
}

void cSphereComponent::SetVelocity(const glm::vec3& vel)
{
	this->mVelocity = vel;
}

void cSphereComponent::UpdateTransform()
{
	transform.pos = this->mPosition;
}

void cSphereComponent::AddForce(const glm::vec3& force)
{
	cRigidBody::AddForce(force);
}

bool cSphereComponent::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cSphereComponent::deserialize(rapidxml::xml_node<>* root_node)
{
	this->mPos = XML_Helper::AsVec3(root_node->first_node("Position"));
	this->mRadius = XML_Helper::AsFloat(root_node->first_node("Radius"));
	return false;
}
