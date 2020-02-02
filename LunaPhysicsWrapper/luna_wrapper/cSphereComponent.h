#pragma once
#include <interfaces/physics/iSphereComponent.h>
#include <luna_physics/cRigidBody.h>

namespace nPhysics
{

	class cSphereComponent : public iSphereComponent, public phys::cRigidBody
	{
	public:
		cSphereComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape);
		virtual ~cSphereComponent();

		// Inherited via iSphereComponent
		virtual bool serialize(rapidxml::xml_node<>* root_node);
		virtual bool deserialize(rapidxml::xml_node<>* root_node);
		virtual void GetTransform(glm::mat4& transformOut) override;
		virtual void AddForce(const glm::vec3& force) override;
		virtual void SetVelocity(const glm::vec3& velocity) override;
		virtual glm::vec3 GetVelocity() override;
		virtual void UpdateTransform() override;

		// Inherited via iSphereComponent
		virtual void AddVelocity(const glm::vec3& velocity) override;
		virtual void SetPosition(const glm::vec3& position) override;
		virtual glm::vec3 GetPosition() override;

		virtual void Collided(cRigidBody* other) override;
	};

}