#include "cSphereComponent.h"
#include "cPhysicsFactory.h"
#include <interfaces/Behaviour/iBehaviour.h>

namespace nPhysics
{
	cSphereComponent::cSphereComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape)
		: cRigidBody(def, shape)
		, iSphereComponent(parent)
	{
		cPhysicsFactory factory;
		factory.GetWorld()->AddComponent(this);
	}

	cSphereComponent::~cSphereComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);
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

	void cSphereComponent::AddVelocity(const glm::vec3& velocity)
	{
		this->mVelocity += velocity;
	}

	void cSphereComponent::SetPosition(const glm::vec3& position)
	{
		this->mPosition = position;
		this->mPreviousPosition = position;
	}

	glm::vec3 cSphereComponent::GetPosition()
	{
		return this->mPosition;
	}

	void cSphereComponent::Collided(cRigidBody* other)
	{
		iBehaviour* behaviour = parent.GetComponent<iBehaviour>();
		if (behaviour)
		{
			behaviour->OnCollide(&dynamic_cast<iPhysicsComponent*>(other)->parent);
		}
	}

}