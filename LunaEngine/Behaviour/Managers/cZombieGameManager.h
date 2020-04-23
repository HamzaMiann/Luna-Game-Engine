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
			ENDING,
			END
		};

		virtual ~cZombieGameManager() {}

		cZombieGameManager(iObject* root) :
			iBehaviour(root)
		{
		}

		virtual void start() override;
		virtual void update(float dt) override;

		void CalculateDarkness();
		void ActivateTomb();

	private:
		float mDt;
		float mDarknessValue;
		STATE mState;

		int tombsActivated = 0;
		float endTime = 5.f;
	};
}