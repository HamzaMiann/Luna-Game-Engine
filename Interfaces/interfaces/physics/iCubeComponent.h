#pragma once


#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sCubeDef : public sComponentDef
	{
		glm::vec3 Scale;
		glm::vec3 Origin;
	};
	class iCubeComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCubeComponent() {}

	protected:
		iCubeComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::cube)
		{
		}

	private:
		iCubeComponent() = delete;
		iCubeComponent(const iCubeComponent& other) = delete;
		iCubeComponent& operator=(const iCubeComponent& other) = delete;
	};
}