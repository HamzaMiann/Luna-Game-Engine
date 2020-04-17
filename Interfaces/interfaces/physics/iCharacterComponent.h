#pragma once


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

		inline void SetWalkDirection(const vec3& direction) { walkDirection = direction; }

		virtual void Walk(float speed) = 0;
		virtual void Jump(const vec3& direction) = 0;
		virtual bool CanJump() = 0;


	protected:

		vec3 walkDirection;

		iCharacterComponent(iObject* parent)
			: iPhysicsComponent(parent, eComponentType::character)
			, walkDirection(vec3(0,0, -1))
		{
		}


	private:
		iCharacterComponent() = delete;
		iCharacterComponent(const iCharacterComponent& other) = delete;
		iCharacterComponent& operator=(const iCharacterComponent& other) = delete;
	};
}
