#include "cCharacterComponent.h"
#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include <interfaces\Behaviour\iBehaviour.h>


namespace nPhysics {

	cCharacterComponent::cCharacterComponent(iObject* parent, const sCharacterDef& def)
		: iCharacterComponent(parent)
	{
		btCapsuleShape* colShape = new btCapsuleShape(def.size.x, def.size.y);
		mGhostObject = new btPairCachingGhostObject;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(this->transform.pos + def.Offset));
		offset = def.Offset;

		mGhostObject->setWorldTransform(transform);
		mGhostObject->setCollisionShape(colShape);
		mGhostObject->setUserPointer(this);
		//mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		btVector3 up = btVector3(0., 1.f, 0.);
		mCharacterController = new btKinematicCharacterController(mGhostObject, colShape, btScalar(1.f), up);

		/// Create Dynamic Objects

		//btScalar mass(def.mass);

		////rigidbody is dynamic if and only if mass is non zero, otherwise static
		//bool isDynamic = (mass != 0.f);

		//btVector3 localInertia(0, 0, 0);
		//if (isDynamic)
		//	colShape->calculateLocalInertia(mass, localInertia);

		////using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		//btDefaultMotionState* myMotionState = new btDefaultMotionState(transform);
		//btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		//mBody = new btRigidBody(rbInfo);
		////mBody = new btKinematicCharacterController

		//rbInfo.m_restitution = 0.7f;

		//mBody->applyCentralImpulse(nConvert::ToBullet(def.velocity));

		//mBody->setUserPointer((iPhysicsComponent*)this);
		//if (!this->_rotateable)
		//{
		//	mBody->setAngularFactor(btVector3(0., 0., 0.));
		//	mBody->setLinearFactor(btVector3(1., 0., 1.));
		//}

		cPhysicsFactory factory;
		dynamic_cast<cPhysicsWorld*>(factory.GetWorld())->AddCharacter(this);
	}

	cCharacterComponent::~cCharacterComponent()
	{
		cPhysicsFactory factory;
		dynamic_cast<cPhysicsWorld*>(factory.GetWorld())->RemoveCharacter(this);

		mGhostObject->setUserPointer(0);
		delete mGhostObject->getCollisionShape();
		delete mCharacterController->getGhostObject();
		delete mCharacterController;
		mGhostObject = 0;
		mCharacterController = 0;
	}

	void cCharacterComponent::AddForce(const vec3& force)
	{
	}

	void cCharacterComponent::AddVelocity(const vec3& velocity)
	{
	}

	void cCharacterComponent::AddForceToPoint(const vec3& force, const vec3& point)
	{
	}

	void cCharacterComponent::SetVelocity(const vec3& velocity)
	{
	}

	vec3 cCharacterComponent::GetVelocity()
	{
		return vec3();
	}

	void cCharacterComponent::SetPosition(const glm::vec3& position)
	{
	}

	vec3 cCharacterComponent::GetPosition()
	{
		btTransform tform(mGhostObject->getWorldTransform());
		return nConvert::ToGLM(tform.getOrigin()) - offset;
	}

	void cCharacterComponent::SetRotation(const quat& rotation)
	{
	}

	quat cCharacterComponent::GetRotation()
	{
		btTransform tform(mGhostObject->getWorldTransform());
		return nConvert::ToGLM(tform.getRotation());
	}

	void cCharacterComponent::UpdateTransform()
	{
		btTransform tf(mGhostObject->getWorldTransform());
		transform.Position(nConvert::ToGLM(tf.getOrigin()) - offset);
		if (_rotateable)
			transform.Rotation(nConvert::ToGLM(tf.getRotation()));
	}

	void cCharacterComponent::GetTransform(mat4& transformOut)
	{
		btTransform tf(mGhostObject->getWorldTransform());
		nConvert::ToSimple(tf, transformOut);
	}

	void cCharacterComponent::CollidedWith(iPhysicsComponent* other)
	{
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