#pragma once

#include "../nConvert.h"
#include <interfaces/physics/iSphereComponent.h>
#include "cBulletWrapperComponent.h"

namespace nPhysics
{
	class cSphereComponent : public iSphereComponent, public cBulletWrapperComponent
	{
	public:
		cSphereComponent(iObject* parent, const sSphereDef& def);
		virtual ~cSphereComponent();

		/*

		SERIALIZATION

		*/
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		/*

		FORCE

		*/
		virtual void AddForce(const vec3& force) override;

		/*

		VELOCITY

		*/
		virtual void AddVelocity(const vec3& velocity) override;
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
	};
}