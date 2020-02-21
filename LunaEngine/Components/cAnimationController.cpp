#include "cAnimationController.h"
#include <Animation/cAnimationManager.h>
using namespace rapidxml;

cAnimationController::~cAnimationController()
{
}

bool cAnimationController::deserialize(xml_node<>* root_node)
{
	std::string profile = root_node->first_attribute("profile")->value();
	animation = cAnimationManager::Instance().profiles[profile];
	if (animation)
	{
		numAnimations = animation->mapAnimationFriendlyNameTo_pScene.size();
		if (numAnimations > 0)
		{
			currentAnimation = animation->mapAnimationFriendlyNameTo_pScene.begin()->first;
		}
	}
	return true;
}

void cAnimationController::start()
{
}

void cAnimationController::update(float dt)
{
	time += dt;
	if (animation)
		animation->BoneTransform(time, currentAnimation, vecFinalTransformation, vecObjectBoneTransformation, vecOffsets);
}

void cAnimationController::SetAnimation(std::string name)
{
	currentAnimation = name;
}

std::string cAnimationController::GetAnimation()
{
	return currentAnimation;
}

float cAnimationController::GetCurrentAnimationTime()
{
	float animTime = 0.f;

	auto itAnimation = animation->mapAnimationFriendlyNameTo_pScene.find(currentAnimation);

	if (itAnimation != animation->mapAnimationFriendlyNameTo_pScene.end())
	{
		float TicksPerSecond = (itAnimation->second.pAIScene->mAnimations[0]->mTicksPerSecond);
		float duration = (itAnimation->second.pAIScene->mAnimations[0]->mDuration);

		float TimeInTicks = time * TicksPerSecond;
		animTime = fmod(TimeInTicks, duration);
	}

	return animTime;
}

float cAnimationController::GetAnimationDuration()
{
	return GetAnimationDuration(currentAnimation);
}

float cAnimationController::GetAnimationDuration(std::string name)
{
	float duration = 0.f;

	auto itAnimation = animation->mapAnimationFriendlyNameTo_pScene.find(name);

	if (itAnimation != animation->mapAnimationFriendlyNameTo_pScene.end())
	{
		float TicksPerSecond = (itAnimation->second.pAIScene->mAnimations[0]->mTicksPerSecond);
		duration = (itAnimation->second.pAIScene->mAnimations[0]->mDuration) / TicksPerSecond;
	}

	return duration;
}

std::vector<std::string> cAnimationController::GetAnimationNames()
{
	std::vector<std::string> names;
	for (auto it = animation->mapAnimationFriendlyNameTo_pScene.begin();
		it != animation->mapAnimationFriendlyNameTo_pScene.end();
		++it)
		names.push_back(it->first);
	return names;
}

const std::vector<mat4>& cAnimationController::GetTransformations()
{
	return vecFinalTransformation;
}

void cAnimationController::SetAnimation(std::string name, float time)
{
	this->time = time;
	currentAnimation = name;
}
