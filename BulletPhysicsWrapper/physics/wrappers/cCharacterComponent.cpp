#include "cCharacterComponent.h"
#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include <interfaces\Behaviour\iBehaviour.h>


namespace nPhysics {

	cCharacterComponent::cCharacterComponent(iObject* parent, const sCharacterDef& def)
		: iCharacterComponent(parent)
	{
		btConvexShape* colShape = new btCapsuleShape(def.size.x, def.size.y);
		mGhostObject = new btPairCachingGhostObject;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(this->transform.pos + def.Offset));
		transform.setRotation(nConvert::ToBullet(quat(vec3(glm::radians(90.f), 0, 0))));
		offset = def.Offset;

		mGhostObject->setWorldTransform(transform);
		mGhostObject->setCollisionShape(colShape);
		mGhostObject->setUserPointer(this);
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_CHARACTER_OBJECT);

		btVector3 up = btVector3(0., 1.f, 0.);
		mCharacterController = new btKinematicCharacterController(mGhostObject, colShape, btScalar(1.f), up);

		dynamic_cast<cPhysicsWorld*>(cPhysicsFactory().GetWorld())->AddCharacter(this);
	}

	cCharacterComponent::~cCharacterComponent()
	{
		dynamic_cast<cPhysicsWorld*>(cPhysicsFactory().GetWorld())->RemoveCharacter(this);

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
		mGhostObject->activate();
		mCharacterController->setWalkDirection(nConvert::ToBullet(velocity));
	}

	void cCharacterComponent::AddForceToPoint(const vec3& force, const vec3& point)
	{
	}

	void cCharacterComponent::SetVelocity(const vec3& velocity)
	{
		mGhostObject->activate();
		mCharacterController->setWalkDirection(nConvert::ToBullet(velocity));
	}

	vec3 cCharacterComponent::GetVelocity()
	{
		return nConvert::ToGLM(mCharacterController->getLinearVelocity());
	}

	void cCharacterComponent::SetPosition(const glm::vec3& position)
	{
		btTransform tform(mGhostObject->getWorldTransform());
		tform.setOrigin(nConvert::ToBullet(position));
		mGhostObject->setWorldTransform(tform);
	}

	vec3 cCharacterComponent::GetPosition()
	{
		btTransform tform(mGhostObject->getWorldTransform());
		return nConvert::ToGLM(tform.getOrigin()) - offset;
	}

	void cCharacterComponent::SetRotation(const quat& rotation)
	{
		btTransform tform(mGhostObject->getWorldTransform());
		tform.setRotation(nConvert::ToBullet(rotation));
		mGhostObject->setWorldTransform(tform);
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
		/*if (_rotateable)
			transform.Rotation(nConvert::ToGLM(tf.getRotation()));*/
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
			if (component->GetComponentType() == ComponentType::Behaviour)
			{
				reinterpret_cast<iBehaviour*>(component)->OnCollide(&other->parent);
			}
		}
	}

	void cCharacterComponent::Walk(float speed)
	{
		mCharacterController->setWalkDirection(nConvert::ToBullet(walkDirection * speed));
	}

	void cCharacterComponent::Jump(const vec3& direction)
	{
		mCharacterController->jump(nConvert::ToBullet(direction));
	}

	bool cCharacterComponent::CanJump()
	{
		return mCharacterController->canJump();
	}

	void cCharacterComponent::SetMaxSlope(float radiansAngle)
	{
		mCharacterController->setMaxSlope(radiansAngle);
	}

	void cCharacterComponent::SetStepHeight(float height)
	{
		mCharacterController->setStepHeight(height);
	}

}