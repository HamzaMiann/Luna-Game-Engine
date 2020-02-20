#pragma once

#include <Animation/cAnimation.h>
#include <map>
#include <string>

class cAnimationManager
{
public:
	~cAnimationManager();

	static cAnimationManager& Instance()
	{
		static cAnimationManager instance;
		return instance;
	}

	std::map<std::string, cAnimation*> animations;

	void UpdateTransforms(cAnimation& animation, float animationTime, std::vector<mat4>& transformations);
};