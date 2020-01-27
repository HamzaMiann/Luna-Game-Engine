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

		virtual void GetTransform(glm::mat4& transformOut) = 0;

		// Inherited via iComponent
		virtual bool serialize(rapidxml::xml_node<>* root_node) = 0;
		virtual bool deserialize(rapidxml::xml_node<>* root_node) = 0;

	protected:
		iPhysicsComponent(iObject* parent, eComponentType componentType)
			: mComponentType(componentType)
			, transform(parent->transform)
		{}
	private:
		eComponentType mComponentType;
		sTransform& transform;

		// get rid of these!
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;

	};
}