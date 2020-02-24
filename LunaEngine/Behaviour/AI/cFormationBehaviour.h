#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#endif

#include <Components/cAnimationController.h>

class cFormationGameManager;

namespace AI
{
	enum class AI_STATE
	{
		NONE,
		FORMATION,
		PATH_FOLLOW,
		FLOCKING,
	};

	class cFormationBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cFormationBehaviour() {}
		cFormationBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{
		}

		nPhysics::iPhysicsComponent* rigidBody;
		cAnimationController* animator;

		cFormationGameManager* manager;

		AI_STATE state = AI_STATE::NONE;

		vec3 target;
		float maxVelocity;
		float slowingRadius;

		// Inherited via iAIBehaviour
		virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }

		virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

		virtual void start() override;

		virtual void update(float dt) override;

		void Seek(float dt);

		void Animate(float length);

		vec3 Flock(float dt);

		vec3 Separation(float dt);

		vec3 Alignment(float dt);

		vec3 Cohesion(float dt);

	};
};