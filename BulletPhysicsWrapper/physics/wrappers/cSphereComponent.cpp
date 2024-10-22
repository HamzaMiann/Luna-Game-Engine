#include "cSphereComponent.h"
#include "cPhysicsFactory.h"
#include <interfaces/Behaviour/iBehaviour.h>

namespace nPhysics
{
	cSphereComponent::cSphereComponent(iObject* parent, const sSphereDef& def)
		: iSphereComponent(parent)
		, cBulletWrapperComponent()
	{
		btCollisionShape* colShape = new btSphereShape(btScalar(def.Radius));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(this->transform.Position() + def.Offset));
		offset = def.Offset;

		/// Create Dynamic Objects

		btScalar mass(def.mass);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		mBody = new btRigidBody(rbInfo);

		rbInfo.m_restitution = 0.7f;

		mBody->applyCentralImpulse(nConvert::ToBullet(def.velocity));

		mBody->setUserPointer((iPhysicsComponent*)this);

		cPhysicsFactory factory;
		factory.GetWorld()->AddComponent(this);
	}

	cSphereComponent::~cSphereComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);

		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	void cSphereComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cSphereComponent::AddForce(const glm::vec3& force)
	{
		mBody->activate(true);
		mBody->applyCentralForce(nConvert::ToBullet(force));
	}

	bool cSphereComponent::serialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	bool cSphereComponent::deserialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	void cSphereComponent::SetVelocity(const glm::vec3& velocity)
	{
		mBody->activate(true);
		mBody->setLinearVelocity(nConvert::ToBullet(velocity));
	}

	glm::vec3 cSphereComponent::GetVelocity()
	{
		return nConvert::ToGLM(mBody->getLinearVelocity());
	}

	void cSphereComponent::UpdateTransform()
	{
		btTransform tf;
		mBody->getMotionState()->getWorldTransform(tf);
		transform.pos = nConvert::ToGLM(tf.getOrigin()) - offset;
		if (_rotateable)
			transform.rotation = nConvert::ToGLM(tf.getRotation());
	}
	void cSphereComponent::AddVelocity(const glm::vec3& velocity)
	{
		mBody->activate(true);
		mBody->applyCentralImpulse(nConvert::ToBullet(velocity));
	}
	void cSphereComponent::AddForceToPoint(const vec3& force, const vec3& point)
	{
		mBody->activate(true);
		mBody->applyForce(nConvert::ToBullet(force), nConvert::ToBullet(point - transform.Position()));
	}
	void cSphereComponent::SetPosition(const glm::vec3& position)
	{
		btTransform tform;
		btMotionState* state;
		state = mBody->getMotionState();
		state->getWorldTransform(tform);
		tform.setOrigin(nConvert::ToBullet(position));
		state->setWorldTransform(tform);
		mBody->setMotionState(state);
		offset = vec3(0.f);
		mBody->activate(true);
	}
	glm::vec3 cSphereComponent::GetPosition()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getOrigin());
	}

	void cSphereComponent::SetRotation(const quat& rotation)
	{
		btTransform tform;
		btMotionState* state;
		state = mBody->getMotionState();
		state->getWorldTransform(tform);
		tform.setRotation(nConvert::ToBullet(rotation));
		state->setWorldTransform(tform);
		mBody->setMotionState(state);
	}

	quat cSphereComponent::GetRotation()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getRotation());
	}

	void cSphereComponent::CollidedWith(iPhysicsComponent* other)
	{
		if (other->GetComponentType() == eComponentType::character)
			mBody->activate();

		for (iComponent* component : parent.Components())
		{
			iBehaviour* behaviour = dynamic_cast<iBehaviour*>(component);
			if (behaviour)
			{
				behaviour->OnCollide(&other->parent);
			}
		}
	}
}