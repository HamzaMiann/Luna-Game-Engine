#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sSphereDef : public sComponentDef
	{
		float Radius;
		glm::vec3 Offset;
	};
	class iSphereComponent : public iPhysicsComponent
	{
	public:
		virtual ~iSphereComponent() {}

	protected:
		iSphereComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::plane)
		{
		}

	private:
		iSphereComponent() = delete;
		iSphereComponent(const iSphereComponent& other) = delete;
		iSphereComponent& operator=(const iSphereComponent& other) = delete;
	};
}