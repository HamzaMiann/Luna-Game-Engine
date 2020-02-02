#include "cPlaneComponent.h"
#include "cPhysicsFactory.h"

namespace nPhysics
{
	cPlaneComponent::cPlaneComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape)
		: cRigidBody(def, shape)
		, iPlaneComponent(parent)
	{
		cPhysicsFactory factory;
		factory.GetWorld()->AddComponent(this);
	}

	cPlaneComponent::~cPlaneComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);
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
		GetTransformFromBody(transformOut);
	}

	void cPlaneComponent::AddForce(const glm::vec3& force)
	{
		AddForceToCenter(force);
	}

	void cPlaneComponent::SetVelocity(const glm::vec3& velocity)
	{
		mVelocity = velocity;
	}

	glm::vec3 cPlaneComponent::GetVelocity()
	{
		return mVelocity;
	}

	void cPlaneComponent::UpdateTransform()
	{
		transform.pos = mPosition;
	}

	void cPlaneComponent::AddVelocity(const glm::vec3& velocity)
	{
		mVelocity += velocity;
	}

	void cPlaneComponent::SetPosition(const glm::vec3& position)
	{
		mPosition = position;
		mPreviousPosition = position;
	}

	glm::vec3 cPlaneComponent::GetPosition()
	{
		return mPosition;
	}

}