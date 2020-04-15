#pragma once


#include <glm/vec3.hpp>
#include "iPhysicsComponent.h"

namespace nPhysics
{
	struct sCharacterDef : public sComponentDef
	{
		glm::vec2 size;
	};
	class iCharacterComponent : public iPhysicsComponent
	{
	public:
		virtual ~iCharacterComponent() {}

	protected:
		iCharacterComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::character)
		{
		}

	private:
		iCharacterComponent() = delete;
		iCharacterComponent(const iCharacterComponent& other) = delete;
		iCharacterComponent& operator=(const iCharacterComponent& other) = delete;
	};
}
