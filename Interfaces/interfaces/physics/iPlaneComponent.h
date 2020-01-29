#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sPlaneDef : public sComponentDef
	{
		float Constant;
		glm::vec3 Normal;
	};
	class iPlaneComponent : public iPhysicsComponent
	{
	public:
		virtual ~iPlaneComponent() {}

	protected:
		iPlaneComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::plane)
		{}

	private:
		iPlaneComponent() = delete;
		iPlaneComponent(const iPlaneComponent& other) = delete;
		iPlaneComponent& operator=(const iPlaneComponent& other) = delete;
	};
}