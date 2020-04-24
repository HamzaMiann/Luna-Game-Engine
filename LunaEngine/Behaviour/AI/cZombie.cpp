#include "cZombie.h"
#include <Physics/Mathf.h>
#include <EntityManager/cEntityManager.h>
#include <Audio/AudioEngine.hpp>

namespace AI
{

	void cZombie::start()
	{
		mState = IDLE;
		health = 30.f;
		player = cEntityManager::Instance().GetObjectByTag("player")->GetComponent<cFPSController>();
		rigidBody = parent.GetComponent<nPhysics::iCharacterComponent>();
		anim = parent.GetComponent<cAnimationController>();
	}

	void cZombie::update(float dt)
	{
		if (!player)
		{
			player = cEntityManager::Instance().GetObjectByTag("player")->GetComponent<cFPSController>();
			return;
		}

		if (mState == IDLE)
		{
			if (glm::distance2(player->parent.transform.Position(), transform.Position()) < 120.f)
			{
				mState = ALERT;
				AudioEngine::Instance()->PlaySound("zombie");
			}
		}
		else if (mState == ALERT)
		{
			vec3 forward(0.f, 0.f, 1.f);
			vec3 direction = player->parent.transform.Position() - transform.Position();
			direction.y = 0.f;
			direction = glm::normalize(direction);
			quat shouldBe = Mathf::RotationFromTo(forward, direction);
			transform.Rotation(glm::lerp(transform.Rotation(), shouldBe, dt * 5.f));

			direction = transform.Rotation() * forward;

			rigidBody->SetWalkDirection(direction);
			rigidBody->Walk(dt * 1.f);
			anim->SetAnimation("walk");
		}
		else if (mState == ATTACK)
		{

		}
		else if (mState == DEAD)
		{
			float duration = anim->GetAnimationDuration();
			if ((anim->GetCurrentTime() + dt) >= duration)
			{
				anim->SetAnimation("die", anim->GetCurrentTime() - dt);
				deathTimer -= dt;
				if (deathTimer <= 0.f)
				{
					cEntityManager::Instance().RemoveEntity(&this->parent);
				}
			}
		}
	}

	void cZombie::TakeDamage(float health)
	{
		this->health -= health;

		if (mState == IDLE)
		{
			mState = ALERT;
			AudioEngine::Instance()->PlaySound("zombie");
		}

		if (this->health <= 0.f && mState != DEAD)
		{
			mState = DEAD;
			anim->SetAnimation("die", 0.f);
			parent.RemoveComponent<nPhysics::iCharacterComponent>();
		}
	}

}
