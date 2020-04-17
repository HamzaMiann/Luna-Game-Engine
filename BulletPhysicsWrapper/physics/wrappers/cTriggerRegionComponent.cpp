#include "cTriggerRegionComponent.h"
#include "cPhysicsFactory.h"
#include "cPhysicsWorld.h"
#include <interfaces/Behaviour/iBehaviour.h>

namespace nPhysics
{
	cTriggerRegionComponent::cTriggerRegionComponent(iObject* parent, const sTriggerDef& def):
		iTriggerRegionComponent(parent)
	{
		btConvexShape* colShape = 0;
		if (def.ShapeType == "sphere") {
			colShape = new btSphereShape(def.Extents.x);
		}
		else
		{
			colShape = new btBoxShape(nConvert::ToBullet(def.Extents));
		}

		mGhostObject = new btPairCachingGhostObject;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(nConvert::ToBullet(this->transform.pos + def.Offset));
		//transform.setRotation(nConvert::ToBullet(quat(vec3(glm::radians(90.f), 0, 0))));
		offset = def.Offset;

		mGhostObject->setWorldTransform(transform);
		mGhostObject->setCollisionShape(colShape);
		mGhostObject->setUserPointer(this);
		mGhostObject->setCollisionFlags(mGhostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

		dynamic_cast<cPhysicsWorld*>(cPhysicsFactory().GetWorld())->AddObject(this);
	}

	cTriggerRegionComponent::~cTriggerRegionComponent()
	{
		dynamic_cast<cPhysicsWorld*>(cPhysicsFactory().GetWorld())->RemoveObject(this);

		mGhostObject->setUserPointer(0);
		delete mGhostObject->getCollisionShape();
		delete mGhostObject;
		mGhostObject = 0;
	}

	void cTriggerRegionComponent::SetPosition(const glm::vec3& position)
	{
		btTransform tform(mGhostObject->getWorldTransform());
		tform.setOrigin(nConvert::ToBullet(position));
		mGhostObject->setWorldTransform(tform);
	}

	vec3 cTriggerRegionComponent::GetPosition()
	{
		btTransform tform(mGhostObject->getWorldTransform());
		return nConvert::ToGLM(tform.getOrigin()) - offset;
	}

	void cTriggerRegionComponent::SetRotation(const quat& rotation)
	{
		btTransform tform(mGhostObject->getWorldTransform());
		tform.setRotation(nConvert::ToBullet(rotation));
		mGhostObject->setWorldTransform(tform);
	}

	quat cTriggerRegionComponent::GetRotation()
	{
		btTransform tform(mGhostObject->getWorldTransform());
		return nConvert::ToGLM(tform.getRotation());
	}

	void cTriggerRegionComponent::UpdateTransform()
	{
		btTransform tf(mGhostObject->getWorldTransform());
		transform.Position(nConvert::ToGLM(tf.getOrigin()) - offset);
		if (_rotateable)
			transform.Rotation(nConvert::ToGLM(tf.getRotation()));
	}

	void cTriggerRegionComponent::GetTransform(mat4& transformOut)
	{
		btTransform tf(mGhostObject->getWorldTransform());
		nConvert::ToSimple(tf, transformOut);
	}

	void cTriggerRegionComponent::CollidedWith(iPhysicsComponent* other)
	{
		for (iComponent* component : parent.Components())
		{
			if (component->GetComponentType() == ComponentType::Behaviour)
			{
				reinterpret_cast<iBehaviour*>(component)->OnCollide(&other->parent);
			}
		}
	}

}