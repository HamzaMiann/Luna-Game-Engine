#pragma once


#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sMeshDef : public sComponentDef
	{
		struct Triangle
		{
			glm::vec3 first, second, third;
		};
		std::vector<Triangle> Triangles;
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