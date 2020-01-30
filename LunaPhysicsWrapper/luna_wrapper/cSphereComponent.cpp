#include "cSphereComponent.h"
#include "cPhysicsFactory.h"

namespace nPhysics
{
	cSphereComponent::cSphereComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape)
		: cRigidBody(def, shape)
		, iSphereComponent(parent)
	{
		cPhysicsFactory factory;
		factory.GetWorld()->AddComponent(this);
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
		GetTransformFromBody(transformOut);
	}

	void cSphereComponent::AddForce(const glm::vec3& force)
	{
		AddForceToCenter(force);
	}

	void cSphereComponent::SetVelocity(const glm::vec3& velocity)
	{
		mVelocity = velocity;
	}

	glm::vec3 cSphereComponent::GetVelocity()
	{
		return mVelocity;
	}

	void cSphereComponent::UpdateTransform()
	{
		transform.pos = mPosition;
	}

}