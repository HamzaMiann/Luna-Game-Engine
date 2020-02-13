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

		void GetTransform(glm::mat4& transformOut);
		//void ApplyForce(const glm::vec3& force);

		// Inherited via iSphereComponent
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;
		virtual void AddForce(const glm::vec3& force) override;
		virtual void SetVelocity(const glm::vec3& velocity) override;
		virtual glm::vec3 GetVelocity() override;
		virtual void UpdateTransform() override;

		// Inherited via iSphereComponent
		virtual void AddVelocity(const glm::vec3& velocity) override;
		virtual void SetPosition(const glm::vec3& position) override;
		virtual glm::vec3 GetPosition() override;

		virtual void CollidedWith(iPhysicsComponent* other) override;
	};
}