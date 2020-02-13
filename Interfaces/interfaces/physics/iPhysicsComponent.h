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
		float gravity_factor;
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

		virtual void AddVelocity(const glm::vec3& velocity) = 0;
		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual glm::vec3 GetPosition() = 0;

		virtual void UpdateTransform() = 0;

		virtual void CollidedWith(iPhysicsComponent* other) {}

		inline void setIsRotateable(bool value) { _rotateable = value; }

		sTransform& transform;
		iObject& parent;

	protected:
		bool _rotateable;

		iPhysicsComponent(iObject* parent, eComponentType componentType)
			: mComponentType(componentType)
			, parent(*parent)
			, transform(parent->transform)
			, _rotateable(true)
		{}

	private:
		eComponentType mComponentType;

		// get rid of these!
		iPhysicsComponent() = delete;
		iPhysicsComponent(const iPhysicsComponent& other) = delete;
		iPhysicsComponent& operator=(const iPhysicsComponent& other) = delete;
	};
}