#include "cPlaneComponent.h"

namespace nPhysics
{
	cPlaneComponent::cPlaneComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape)
		: cRigidBody(def, shape)
		, iPlaneComponent(parent)
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

	void cPlaneComponent::GetTransform(glm::mat4& transformOut)
	{
		cRigidBody::GetTransform(transformOut);
	}

	void cPlaneComponent::AddForce(const glm::vec3& force)
	{
		cRigidBody::AddForce(force);
	}

	void cPlaneComponent::SetVelocity(const glm::vec3& velocity)
	{
		cRigidBody::mVelocity = velocity;
	}

	glm::vec3 cPlaneComponent::GetVelocity()
	{
		return cRigidBody::mVelocity;
	}

	void cPlaneComponent::UpdateTransform()
	{
		transform.pos = cRigidBody::mVelocity;
	}

}