#include "cZombieGameManager.h"
#include <Audio/AudioEngine.hpp>
#include <Rendering/RenderingEngine.h>

void AI::cZombieGameManager::start()
{
	mDarknessValue = 0.f;
	AudioEngine::Instance()->PlaySound("intro");
}

void AI::cZombieGameManager::update(float dt)
{
	if (mDarknessValue > 0.22f)
	{
		mDarknessValue += dt * 0.7f;
		RenderingEngine::Instance().UITexture.SetTexture("Intro.png");
	}
	else mDarknessValue += dt * 0.05f;
	if (mDarknessValue > 1.f) mDarknessValue = 1.f;
	RenderingEngine::Instance().SetProperty("mixValue", mDarknessValue);
}
