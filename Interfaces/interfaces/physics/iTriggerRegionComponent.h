#pragma once
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sTriggerDef : public sComponentDef
	{
		std::string ShapeType;
		vec3 Extents;
	};
	class iTriggerRegionComponent : public iPhysicsComponent
	{
	public:
		virtual ~iTriggerRegionComponent() {}

	protected:
		iTriggerRegionComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::trigger)
		{
		}

	private:
		iTriggerRegionComponent() = delete;
		iTriggerRegionComponent(const iTriggerRegionComponent& other) = delete;
		iTriggerRegionComponent& operator=(const iTriggerRegionComponent& other) = delete;
	};
}