#include "cZombieGameManager.h"
#include <Audio/AudioEngine.hpp>
#include <Rendering/RenderingEngine.h>
#include <InputManager.h>
#include <EntityManager/cEntityManager.h>
#include <Behaviour/Controls/cFPSController.h>

namespace AI
{

	void cZombieGameManager::start()
	{
		mState = INTRO;
		mDarknessValue = 0.f;
		AudioEngine::Instance()->PlaySound("intro");
		AudioEngine::Instance()->PlaySound("wind");

		Camera::main_camera->Eye = vec3(27.1113186f, 0.150644f, -136.474823f);
		Camera::main_camera->Target = vec3(25.9632452f, 7.42755f, -35.5345f);
		Camera::main_camera->Up = glm::normalize(vec3(0.f, 1.f, 0.f) + vec3(0.1f, 0.f, 0.f));
	}

	void cZombieGameManager::CalculateDarkness()
	{
		if (mDarknessValue > 0.15f)
		{
			mDarknessValue += mDt * 0.7f;
		}
		else mDarknessValue += mDt * 0.03f;
		if (mDarknessValue > 1.f) mDarknessValue = 1.f;
		RenderingEngine::Instance().SetProperty("mixValue", mDarknessValue);
	}

	void cZombieGameManager::ActivateTomb()
	{
		tombsActivated++;
		if (tombsActivated == 4)
		{
			AudioEngine::Instance()->GetSound("wind")->toggle_pause();
			AudioEngine::Instance()->PlaySound("glitch");
			mState = ENDING;
		}
	}

	void cZombieGameManager::update(float dt)
	{
		mDt = dt;
		CalculateDarkness();
		
		if (mState == INTRO)
		{

			if (mDarknessValue > 0.8f)
			{
				RenderingEngine::Instance().UITexture.SetTexture("Intro.png");
				if (Input::KeyDown(GLFW_KEY_ENTER))
				{
					mState = STARTED;

					auto* component = cEntityManager::Instance().GetObjectByTag("player")->AddComponent<cFPSController>();
					cEntityManager::Instance().RemoveEntity(cEntityManager::Instance().GetObjectByTag("introZombie"));
					cFPSController::Settings settings;
					settings.forward = vec3(0.f, 0.f, -1.f);
					settings.camera_offset = vec3(0.7f, 5.f, 0.f);
					settings.distance_from_object = 4.f;
					settings.speed = 5.f;
					settings.mouse_speed = 3.14f;
					component->SetSettings(settings);

					Camera::main_camera->Up = vec3(0.f, 1.f, 0.f);
				}
			}
		}
		else if (mState == STARTED)
		{

		}
		else if (mState == ENDING)
		{
			endTime -= dt;
			if (endTime <= 0.f)
			{
				for (auto* object : cEntityManager::Instance().GetEntities())
				{
					if (object->tag != this->parent.tag)
						cEntityManager::Instance().RemoveEntity(object);
				}
				mState = END;
			}
			else
			{
				RenderingEngine::Instance().UITexture.SetTexture("glitch.png");
			}
		}
		else if (mState == END)
		{
			RenderingEngine::Instance().UITexture.SetTexture("End.png");
		}

	}

}