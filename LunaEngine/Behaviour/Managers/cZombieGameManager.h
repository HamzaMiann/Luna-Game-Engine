#pragma once

#include <interfaces/Behaviour/iBehaviour.h>

namespace AI
{
	class cZombieGameManager : public iBehaviour
	{
	public:

		virtual ~cZombieGameManager() {}

		cZombieGameManager(iObject* root) :
			iBehaviour(root)
		{
		}

		virtual void start() override;
		virtual void update(float dt) override;

	private:
		float mDarknessValue;

	};
}