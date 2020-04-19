#include "cCapsuleComponent.h"
#include "cPhysicsFactory.h"
#include "../nConvert.h"
#include <interfaces/Behaviour/iBehaviour.h>
#include "cPhysMeshComponent.h"

namespace nPhysics 
{

	cCapsuleComponent::cCapsuleComponent(iObject* parent, const sCapsuleDef& def)
		: iCapsuleComponent(parent)
	{
		btCollisionShape* colShape = new btCapsuleShape(def.Scale.x, def.Scale.y);

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(this->transform.pos + def.Offset));
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
		if (!this->_rotateable)
		{
			mBody->setAngularFactor(btVector3(0., 0., 0.));
			mBody->setLinearFactor(btVector3(1., 0., 1.));
		}

		cPhysicsFactory factory;
		factory.GetWorld()->AddComponent(this);
	}

	cCapsuleComponent::~cCapsuleComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);

		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	bool cCapsuleComponent::serialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	bool cCapsuleComponent::deserialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	void cCapsuleComponent::AddForce(const vec3& force)
	{
		mBody->activate(true);
		mBody->applyCentralForce(nConvert::ToBullet(force));
	}

	void cCapsuleComponent::AddVelocity(const vec3& velocity)
	{
		mBody->activate(true);
		mBody->applyCentralImpulse(nConvert::ToBullet(velocity));
	}

	void cCapsuleComponent::SetVelocity(const vec3& velocity)
	{
		mBody->activate(true);
		mBody->setLinearVelocity(nConvert::ToBullet(velocity));
	}

	vec3 cCapsuleComponent::GetVelocity()
	{
		return nConvert::ToGLM(mBody->getLinearVelocity());
	}

	void cCapsuleComponent::SetPosition(const glm::vec3& position)
	{
		btTransform tform;
		btMotionState* state;
		state = mBody->getMotionState();
		state->getWorldTransform(tform);
		tform.setOrigin(nConvert::ToBullet(position));
		state->setWorldTransform(tform);
		mBody->setMotionState(state);
		offset = vec3(0.f);
	}

	vec3 cCapsuleComponent::GetPosition()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getOrigin()) - offset;
	}

	void cCapsuleComponent::SetRotation(const quat& rotation)
	{
		btTransform tform;
		btMotionState* state;
		state = mBody->getMotionState();
		state->getWorldTransform(tform);
		tform.setRotation(nConvert::ToBullet(rotation));
		state->setWorldTransform(tform);
		mBody->setMotionState(state);
	}

	quat cCapsuleComponent::GetRotation()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getRotation());
	}

	void cCapsuleComponent::UpdateTransform()
	{
		btTransform tf;
		mBody->getMotionState()->getWorldTransform(tf);
		transform.Position(nConvert::ToGLM(tf.getOrigin()) - offset);
		if (_rotateable)
			transform.Rotation(nConvert::ToGLM(tf.getRotation()));
	}

	void cCapsuleComponent::GetTransform(mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cCapsuleComponent::CollidedWith(iPhysicsComponent* other)
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