#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#include <Behaviour/Controls/cFPSController.h>
#endif


namespace AI
{
	class cZombie : public iAIBehaviour
	{
	public:
		virtual ~cZombie() {}
		cZombie(iObject* parent)
			: iAIBehaviour(parent)
		{ }

		// Inherited via iAIBehaviour

		virtual void start() override;

		virtual void update(float dt) override;

		void TakeDamage(float health);

		enum STATE
		{
			IDLE,
			ALERT,
			ATTACK,
			DEAD
		} mState;

	private:
		float health;
		cFPSController* player;
		nPhysics::iCharacterComponent* rigidBody;
		cAnimationController* anim;

		float deathTimer = 3.f;
	};
};