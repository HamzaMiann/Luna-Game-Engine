#pragma once

#include "../nConvert.h"
#include "cBulletWrapperComponent.h"
#include <interfaces/physics/iTriggerRegionComponent.h>
#include "../bullet/BulletCollision/CollisionDispatch/btGhostObject.h"

namespace nPhysics
{
	class cTriggerRegionComponent : public iTriggerRegionComponent
	{
	public:

		btPairCachingGhostObject* mGhostObject = 0;

		cTriggerRegionComponent(iObject* parent, const sTriggerDef& def);
		virtual ~cTriggerRegionComponent();

		/*

		SERIALIZATION

		*/
		virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

		/*

		FORCE

		*/
		virtual void AddForce(const vec3& force) override {}

		/*

		VELOCITY

		*/
		virtual void AddVelocity(const vec3& velocity) override {}
		virtual void SetVelocity(const vec3& velocity) override {}
		virtual vec3 GetVelocity() override { return vec3(0.f); }

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

	};
}