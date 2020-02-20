#include "cAnimationManager.h"

cAnimationManager::~cAnimationManager()
{
	auto it = animations.begin();
	while (it != animations.end())
	{
		if (it->second != nullptr)
		{
			delete it->second;
		}
		++it;
	}
	animations.clear();
}

unsigned int GetFrame(cAnimation::sBone& node, float AnimationTime)
{
	int i = 0;
	for (; i < node.mTimes.size(); ++i)
	{
		if (node.mTimes[i] > AnimationTime)
		{
			break;
		}
	}

	i -= 1;
	if (i < 0) i = 0;

	return i;
}

void ReadNodeHeirarchy(float AnimationTime, cAnimation::sBone& node, const mat4& parent, std::vector<mat4>& transformations, const mat4& inverse)
{
	unsigned int frame = GetFrame(node, AnimationTime);

	mat4 base = mat4(1.0f);
	//base = node.nodeTransform;

	base *= glm::translate(mat4(1.f), node.positions[frame]);
	base *= mat4(node.rotations[frame]);
	base *= glm::scale(mat4(1.f), node.scalings[frame]);
	base = parent * base;

	transformations[node.id] = base;

	for (unsigned int i = 0; i < node.children.size(); ++i)
	{
		ReadNodeHeirarchy(AnimationTime, *node.children[i], base, transformations, inverse);
	}
}

void cAnimationManager::UpdateTransforms(cAnimation& animation, float animationTime, std::vector<mat4>& transformations)
{
	float TicksPerSecond = animation.ticksPerSecond;

	float TimeInTicks = animationTime * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)animation.duration);

	transformations.resize(animation.numBones);
	
	mat4 Identity(1.0f);

	ReadNodeHeirarchy(AnimationTime, animation.root, Identity, transformations, animation.GlobalInverse);

}
