#pragma once

#include <interfaces/Behaviour/iBehaviour.h>

namespace AI
{
	class cZombieGameManager : public iBehaviour
	{
	public:

		enum STATE
		{
			INTRO,
			STARTED,
			WAVES
		};

		virtual ~cZombieGameManager() {}

		cZombieGameManager(iObject* root) :
			iBehaviour(root)
		{
		}

		virtual void start() override;
		virtual void update(float dt) override;

		void CalculateDarkness();

	private:
		float mDt;
		float mDarknessValue;
		STATE mState;
	};
}