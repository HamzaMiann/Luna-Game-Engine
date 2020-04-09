#pragma once


#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sMeshDef : public sComponentDef
	{
		std::vector<glm::vec3> vertices;
	};

	class iPhysMeshComponent : public iPhysicsComponent
	{
	public:
		virtual ~iPhysMeshComponent() {}

	protected:
		iPhysMeshComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::mesh)
		{
		}

	private:
		iPhysMeshComponent() = delete;
		iPhysMeshComponent(const iPhysMeshComponent& other) = delete;
		iPhysMeshComponent& operator=(const iPhysMeshComponent& other) = delete;
	};
}