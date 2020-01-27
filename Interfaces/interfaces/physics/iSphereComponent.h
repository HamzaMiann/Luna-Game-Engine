#pragma once
#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sSphereDef
	{
		float Mass;
		float Radius;
		glm::vec3 Position;
	};

	class iSphereComponent : public iPhysicsComponent
	{
	public:
		virtual ~iSphereComponent() {}
		
	protected:
		iSphereComponent(iObject* parent) : iPhysicsComponent(parent, eComponentType::sphere) {}
	private:
		iSphereComponent() = delete;
		iSphereComponent(const iSphereComponent& other) = delete;
		iSphereComponent& operator=(const iSphereComponent& other) = delete;
	};
}