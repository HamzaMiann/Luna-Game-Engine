#pragma once
#include <interfaces/iComponent.h>
#include <interfaces/physics/eComponentType.h>
#include <iObject.h>

namespace nPhysics
{
	class iPhysicsComponent : iComponent
	{
	public:
		virtual ~iPhysicsComponent() {}
		inline const eComponentType& GetComponentType() { return mComponentType; }

		virtual const glm::vec3& GetPosition() = 0;
		virtual void SetPosition(const glm::vec3& pos) = 0;

		virtual const glm::vec3& GetVelocity() = 0;
		virtual void SetVelocity(const glm::vec3& vel) = 0;

		virtual void UpdateTransform() = 0;

		virtual void AddForce(const glm::vec3& force) = 0;


		virtual bool serialize(rapidxml::xml_node<>* root_node) = 0;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;

	protected:
		iPhysicsComponent(iObject* parent, eComponentType componentType)
			: mComponentType(componentType)
			, transform(parent->transform)
		{}

		sTransform& transform;

	private:
		eComponentType mComponentType;

		// get rid of these!
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;

	};
}