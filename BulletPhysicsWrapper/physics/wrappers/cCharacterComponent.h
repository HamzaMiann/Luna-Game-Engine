#pragma once

#include "../nConvert.h"
#include "cBulletWrapperComponent.h"
#include <interfaces/physics/iCharacterComponent.h>
#include "../bullet/BulletDynamics/Character/btKinematicCharacterController.h"
#include "../bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cCharacterComponent : public iCharacterComponent
	{
	public:

		btPairCachingGhostObject* mGhostObject = 0;
		btKinematicCharacterController* mCharacterController = 0;

		cCharacterComponent(iObject* parent, const sCharacterDef& def);
		virtual ~cCharacterComponent();

		/*

		SERIALIZATION

		*/
		virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

		/*

		FORCE

		*/
		virtual void AddForce(const vec3& force) override;

		/*

		VELOCITY

		*/
		virtual void AddVelocity(const vec3& velocity) override;
		virtual void AddForceToPoint(const vec3& force, const vec3& point) override;
		virtual void SetVelocity(const vec3& velocity) override;
		virtual vec3 GetVelocity() override;

		/*

		POSITION

		*/
		virtual void SetPosition(const glm::vec3& position) override;
		virtual vec3 GetPosition() override;

		/*

		ROTATION

		*/
		virtual void SetRotation(const quat& rotation) override;
		virtual quat GetRotation() override;

		/*

		TRANSFORM

		*/
		virtual void UpdateTransform() override;
		virtual void GetTransform(mat4& transformOut) override;


		/*

		COLLISION

		*/
		void CollidedWith(iPhysicsComponent* other) override;

		/*
		
		CHARACTER CONTROLLER FUNCTIONS

		*/
		virtual void Walk(float speed) override;
		virtual void Jump(const vec3& direction) override;
		virtual bool CanJump() override;
		virtual void SetMaxSlope(float radiansAngle) override;
		virtual void SetStepHeight(float height) override;

	};
}