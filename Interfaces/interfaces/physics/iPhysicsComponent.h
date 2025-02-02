#pragma once
#include <glm/glm_common.h>
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
		glm::vec3 Offset;
	};
	class iPhysicsComponent : public iComponent
	{
	public:
		virtual ~iPhysicsComponent() {}
		inline const eComponentType& GetComponentType() { return mComponentType; }

		virtual void GetTransform(mat4& transformOut) = 0;

		virtual void AddForce(const vec3& force) = 0;
		virtual void AddForceToPoint(const vec3& force, const vec3& point) { AddForce(force); }

		virtual void AddVelocity(const vec3& velocity) = 0;
		virtual void SetVelocity(const vec3& velocity) = 0;
		virtual vec3 GetVelocity() = 0;

		virtual void SetPosition(const vec3& position) = 0;
		virtual vec3 GetPosition() = 0;

		virtual void SetRotation(const quat& rotation) = 0;
		virtual quat GetRotation() = 0;

		virtual void SetOffset(const vec3& newOffset) { offset = newOffset; }
		virtual inline vec3 GetOffset() { return offset; }

		virtual void UpdateTransform() = 0;

		virtual void CollidedWith(iPhysicsComponent* other) {}

		virtual void AddHingeConstraint(const vec3& pivot, const vec3& axis) {}
		virtual void AddSliderConstraint(const vec2& linLimit, const vec2& angleLimit) {}

		inline void setIsRotateable(bool value) { _rotateable = value; }

		sTransform& transform;
		iObject& parent;

	protected:
		bool _rotateable;
		vec3 offset;

		iPhysicsComponent(iObject* parent, eComponentType componentType)
			: mComponentType(componentType)
			, iComponent(ComponentType::Physics)
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