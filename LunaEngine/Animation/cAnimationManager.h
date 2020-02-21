#pragma once

#include <Animation/cAnimation.h>
#include <Animation/cSimpleAssimpSkinnedMeshLoader_OneMesh.h>
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
	std::map<std::string, cSimpleAssimpSkinnedMesh*> profiles;

	void UpdateTransforms(cAnimation& animation, float animationTime, std::vector<mat4>& transformations);

};