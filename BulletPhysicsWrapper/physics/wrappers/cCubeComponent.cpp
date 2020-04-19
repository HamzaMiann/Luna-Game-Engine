#include "cCubeComponent.h"
#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include <interfaces/Behaviour/iBehaviour.h>

namespace nPhysics
{
	cCubeComponent::cCubeComponent(iObject* parent, const sCubeDef& def)
		: iCubeComponent(parent)
	{
		baseRotation = this->parent.transform.rotation;

		btCollisionShape* colShape = new btBoxShape(nConvert::ToBullet(def.Scale));

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

	cCubeComponent::~cCubeComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);

		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	bool cCubeComponent::serialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	bool cCubeComponent::deserialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	void cCubeComponent::GetTransform(glm::mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cCubeComponent::CollidedWith(iPhysicsComponent* other)
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

	void cCubeComponent::AddHingeConstraint(const vec3& pivot, const vec3& axis)
	{
		btTypedConstraint* constraint = new btHingeConstraint(*this->mBody, nConvert::ToBullet(pivot), nConvert::ToBullet(axis));
		reinterpret_cast<cPhysicsWorld*>(cPhysicsFactory().GetWorld())->AddConstraint(constraint);
	}

	void cCubeComponent::AddSliderConstraint(const vec2& linLimit, const vec2& angleLimit)
	{
		btTransform tform;
		tform.setIdentity();
		btSliderConstraint* constraint = new btSliderConstraint(*this->mBody, tform, true);
		constraint->setLowerLinLimit(linLimit.x);
		constraint->setUpperLinLimit(linLimit.y);
		constraint->setLowerAngLimit(angleLimit.x);
		constraint->setUpperAngLimit(angleLimit.y);
		reinterpret_cast<cPhysicsWorld*>(cPhysicsFactory().GetWorld())->AddConstraint(constraint);
	}

	void cCubeComponent::AddForce(const glm::vec3& force)
	{
		mBody->activate(true);
		mBody->applyCentralForce(nConvert::ToBullet(force));
	}

	void cCubeComponent::SetVelocity(const glm::vec3& velocity)
	{
		mBody->activate(true);
		mBody->setLinearVelocity(nConvert::ToBullet(velocity));
	}

	glm::vec3 cCubeComponent::GetVelocity()
	{
		return nConvert::ToGLM(mBody->getLinearVelocity());
	}

	void cCubeComponent::AddVelocity(const glm::vec3& velocity)
	{
		mBody->applyCentralImpulse(nConvert::ToBullet(velocity));
	}

	void cCubeComponent::AddForceToPoint(const vec3& force, const vec3& point)
	{
		mBody->activate(true);
		mBody->applyForce(nConvert::ToBullet(force), nConvert::ToBullet(point - transform.Position()));
	}

	void cCubeComponent::SetPosition(const glm::vec3& position)
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

	glm::vec3 cCubeComponent::GetPosition()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getOrigin()) - offset;
	}

	void cCubeComponent::UpdateTransform()
	{
		btTransform tf;
		mBody->getMotionState()->getWorldTransform(tf);
		transform.Position(nConvert::ToGLM(tf.getOrigin()) - offset);
		if (_rotateable)
			transform.Rotation(baseRotation * nConvert::ToGLM(tf.getRotation()));
	}

	void cCubeComponent::SetRotation(const quat& rotation)
	{
		btTransform tform;
		btMotionState* state;
		state = mBody->getMotionState();
		state->getWorldTransform(tform);
		tform.setRotation(nConvert::ToBullet(rotation));
		state->setWorldTransform(tform);
		mBody->setMotionState(state);
	}

	quat cCubeComponent::GetRotation()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getRotation());
	}

}