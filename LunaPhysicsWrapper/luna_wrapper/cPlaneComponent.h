#pragma once

#include <interfaces/physics/iPlaneComponent.h>
#include <luna_physics/cRigidBody.h>

namespace nPhysics
{

	class cPlaneComponent : public iPlaneComponent, public phys::cRigidBody
	{
	public:
		cPlaneComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape);


		// Inherited via iSphereComponent
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;
		virtual void GetTransform(glm::mat4& transformOut) override;
		virtual void AddForce(const glm::vec3& force) override;
		virtual void SetVelocity(const glm::vec3& velocity) override;
		virtual glm::vec3 GetVelocity() override;
		virtual void UpdateTransform() override;

	};

}