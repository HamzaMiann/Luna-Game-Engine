#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sCapsuleDef : public sComponentDef
	{
		glm::vec2 Scale; // width and height
		glm::vec3 Origin;
	};
	class iCapsuleComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCapsuleComponent() {}

	protected:
		iCapsuleComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::capsule)
		{
		}

	private:
		iCapsuleComponent() = delete;
		iCapsuleComponent(const iCapsuleComponent& other) = delete;
		iCapsuleComponent& operator=(const iCapsuleComponent& other) = delete;
	};
}