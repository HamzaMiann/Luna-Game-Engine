#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <interfaces/iComponent.h>
#include <iObject.h>
#include "eComponentType.h"

namespace nPhysics
{
	struct sComponentDef
	{
		glm::vec3 velocity;
		float mass;
	};
	class iPhysicsComponent : public iComponent
	{
	public:
		virtual ~iPhysicsComponent() {}
		inline const eComponentType& GetComponentType() { return mComponentType; }

		virtual void GetTransform(glm::mat4& transformOut) = 0;

		virtual void AddForce(const glm::vec3& force) = 0;
		virtual void SetVelocity(const glm::vec3& velocity) = 0;
		virtual glm::vec3 GetVelocity() = 0;

		virtual void UpdateTransform() = 0;

		sTransform& transform;
		iObject& parent;

	protected:
		iPhysicsComponent(iObject* parent, eComponentType componentType)
			: mComponentType(componentType)
			, parent(*parent)
			, transform(parent->transform)
		{}

	private:
		eComponentType mComponentType;

		// get rid of these!
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;
	};
}