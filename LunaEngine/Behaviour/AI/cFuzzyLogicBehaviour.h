#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#include <interfaces/physics/iPhysicsComponent.h>
#endif


namespace AI
{
	class cFuzzyLogicBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cFuzzyLogicBehaviour() {}
		cFuzzyLogicBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{
		}

		virtual void start() override;

		virtual void update(float dt) override;

		void CalculateDirections();

		void Turn();

		void CalculateSpeed();

		void OnDestroy() override;

	private:

		nPhysics::iPhysicsComponent* rigidBody = 0;
		vec3* directions = 0;
		float* distances = 0;

		float speed;
		float turn;

		float mDt;
		float mTurnSpeed;
		float mMovementSpeed;
	};
};