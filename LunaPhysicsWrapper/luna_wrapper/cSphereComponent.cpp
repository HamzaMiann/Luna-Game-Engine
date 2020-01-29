#include "cSphereComponent.h"

namespace nPhysics
{
	cSphereComponent::cSphereComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape)
		: cRigidBody(def, shape)
		, iSphereComponent(parent)
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

	void cSphereComponent::GetTransform(glm::mat4& transformOut)
	{
		cRigidBody::GetTransform(transformOut);
	}

	void cSphereComponent::AddForce(const glm::vec3& force)
	{
		cRigidBody::AddForce(force);
	}

	void cSphereComponent::SetVelocity(const glm::vec3& velocity)
	{
		cRigidBody::mVelocity = velocity;
	}

	glm::vec3 cSphereComponent::GetVelocity()
	{
		return cRigidBody::mVelocity;
	}

	void cSphereComponent::UpdateTransform()
	{
		transform.pos = cRigidBody::mPosition;
	}

}