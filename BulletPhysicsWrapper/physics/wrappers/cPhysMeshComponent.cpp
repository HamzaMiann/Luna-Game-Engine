#include "cPhysMeshComponent.h"
#include "cPhysicsFactory.h"
#include "../nConvert.h"

namespace nPhysics
{

	cPhysMeshComponent::cPhysMeshComponent(iObject* parent, const sMeshDef& def)
		: iPhysMeshComponent(parent)
	{
		btCollisionShape* colShape = 0;// new btBvhTriangleMeshShape();
		auto trimesh = new btTriangleMesh();

		for (size_t i = 0; i < def.vertices.size() / 3u; ++i)
		{
			size_t index = i * 3u;
			trimesh->addTriangle(
				nConvert::ToBullet(def.vertices[index]),
				nConvert::ToBullet(def.vertices[index + 1u]),
				nConvert::ToBullet(def.vertices[index + 2u]),
				true
				);
		}

		//colShape = new btConvexTriangleMeshShape(trimesh, true);
		colShape = new btBvhTriangleMeshShape(trimesh, true);

		/*auto shape = new btConvexHullShape();
		for (size_t i = 0; i < def.vertices.size() - 1; ++i)
		{
			shape->addPoint(nConvert::ToBullet(def.vertices[i]), true);
		}

		shape->addPoint(nConvert::ToBullet(def.vertices[def.vertices.size() - 1]), true);
		colShape = shape;*/
		
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

	cPhysMeshComponent::~cPhysMeshComponent()
	{
		cPhysicsFactory factory;
		factory.GetWorld()->RemoveComponent(this);

		mBody->setUserPointer(0);
		delete mBody->getCollisionShape();
		delete mBody->getMotionState();
		delete mBody;
		mBody = 0;
	}

	bool cPhysMeshComponent::serialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	bool cPhysMeshComponent::deserialize(rapidxml::xml_node<>* root_node)
	{
		return false;
	}

	void cPhysMeshComponent::AddForce(const vec3& force)
	{
		mBody->activate(true);
		mBody->applyCentralForce(nConvert::ToBullet(force));
	}

	void cPhysMeshComponent::AddVelocity(const vec3& velocity)
	{
		mBody->activate(true);
		mBody->applyCentralImpulse(nConvert::ToBullet(velocity));
	}

	void cPhysMeshComponent::SetVelocity(const vec3& velocity)
	{
		mBody->activate(true);
		mBody->setLinearVelocity(nConvert::ToBullet(velocity));
	}

	vec3 cPhysMeshComponent::GetVelocity()
	{
		return nConvert::ToGLM(mBody->getLinearVelocity());
	}

	void cPhysMeshComponent::SetPosition(const glm::vec3& position)
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

	vec3 cPhysMeshComponent::GetPosition()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getOrigin()) - offset;
	}

	void cPhysMeshComponent::SetRotation(const quat& rotation)
	{
		btTransform tform;
		btMotionState* state;
		state = mBody->getMotionState();
		state->getWorldTransform(tform);
		tform.setRotation(nConvert::ToBullet(rotation));
		state->setWorldTransform(tform);
		mBody->setMotionState(state);
	}

	quat cPhysMeshComponent::GetRotation()
	{
		btTransform tform;
		mBody->getMotionState()->getWorldTransform(tform);
		return nConvert::ToGLM(tform.getRotation());
	}

	void cPhysMeshComponent::UpdateTransform()
	{
		btTransform tf;
		mBody->getMotionState()->getWorldTransform(tf);
		transform.Position(nConvert::ToGLM(tf.getOrigin()) - offset);
		if (_rotateable)
			transform.Rotation(nConvert::ToGLM(tf.getRotation()));
	}

	void cPhysMeshComponent::GetTransform(mat4& transformOut)
	{
		btTransform transform;
		mBody->getMotionState()->getWorldTransform(transform);
		nConvert::ToSimple(transform, transformOut);
	}

	void cPhysMeshComponent::CollidedWith(iPhysicsComponent* other)
	{
		/*for (iComponent* component : parent.Components())
		{
			iBehaviour* behaviour = dynamic_cast<iBehaviour*>(component);
			if (behaviour)
			{
				behaviour->OnCollide(&other->parent);
			}
		}*/
	}


}