#pragma once

#include "iPhysicsComponent.h"

namespace nPhysics
{
	class iEmptyComponent : public iPhysicsComponent
	{
	public:
		virtual ~iEmptyComponent() {}

	protected:
		iEmptyComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::none)
		{
		}

	private:
		iEmptyComponent() = delete;
		iEmptyComponent(const iEmptyComponent& other) = delete;
		iEmptyComponent& operator=(const iEmptyComponent& other) = delete;
	};
}