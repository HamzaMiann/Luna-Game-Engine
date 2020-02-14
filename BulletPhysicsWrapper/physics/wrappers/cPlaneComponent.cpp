#include "cPlaneComponent.h"
#include "cPhysicsFactory.h"

namespace nPhysics
{
	cPlaneComponent::cPlaneComponent(iObject* parent, const sPlaneDef& def)
		: iPlaneComponent(parent)
		, cBulletWrapperComponent()
	{
		btCollisionShape* colShape = new btStaticPlaneShape(nConvert::ToBullet(def.Normal), def.Constant);

		btTransform transform;
		transform.setIdentity();

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.f, myMotionState, colShape, btVector3(0.f, 0.f, 0.f));
		mBody = new btRigidBody(rbInfo);
		rbInfo.m_restitution = 0.7f;

		mBody->setUserPointer((iPhysicsComponent*)this);

		cPhysicsFactory factory;
		factory.GetWorld()->AddComponent(this);
	}

	cPlaneComponent::~cPlaneComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);

		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	void cPlaneComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cPlaneComponent::AddForce(const glm::vec3& force)
	{
		// pass
	}

	bool cPlaneComponent::serialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	bool cPlaneComponent::deserialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	void cPlaneComponent::SetVelocity(const glm::vec3& velocity)
	{
		// pass
	}

	glm::vec3 cPlaneComponent::GetVelocity()
	{
		return glm::vec3(0.f);
	}

	void cPlaneComponent::UpdateTransform()
	{
		// pass
	}
	void cPlaneComponent::AddVelocity(const glm::vec3& velocity)
	{
	}
	void cPlaneComponent::SetPosition(const glm::vec3& position)
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		tform.setOrigin(nConvert::ToBullet(position));
		mBody->getMotionState()->setWorldTransform(tform);
	}
	glm::vec3 cPlaneComponent::GetPosition()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getOrigin());
	}
}
