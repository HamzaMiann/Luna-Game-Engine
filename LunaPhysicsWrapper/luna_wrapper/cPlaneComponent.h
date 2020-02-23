#pragma once

#include <interfaces/physics/iPlaneComponent.h>
#include <luna_physics/cRigidBody.h>

namespace nPhysics
{

	class cPlaneComponent : public iPlaneComponent, public phys::cRigidBody
	{
	public:
		cPlaneComponent(iObject* parent, const phys::sRigidBodyDef& def, phys::iShape* shape);
		virtual ~cPlaneComponent();

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
		virtual void SetRotation(const quat& rotation) override {}
		virtual quat GetRotation() override { return quat(vec3(0.f)); }

		/*

		TRANSFORM

		*/
		virtual void UpdateTransform() override;
		virtual void GetTransform(mat4& transformOut) override;

	};

}