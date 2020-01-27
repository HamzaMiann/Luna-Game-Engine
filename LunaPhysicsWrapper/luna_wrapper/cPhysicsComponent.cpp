#include "cPhysicsComponent.h"

void cPhysicsComponent::GetTransform(glm::mat4& transformOut)
{
}

bool cPhysicsComponent::serialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

bool cPhysicsComponent::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
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
