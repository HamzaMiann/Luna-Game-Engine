#include "cPhysicsComponent.h"
#include <luna_physics/shapes.h>

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
	return false;
}
