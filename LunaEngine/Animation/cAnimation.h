#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/glm_common.h>

struct sBoneInfo
{
	std::string BoneName;
	unsigned int id;
	mat4 BoneOffset;
	mat4 BoneTransform;
	std::string parentBoneName;
};

class cAnimation
{
public:

	struct sBone
	{
		~sBone()
		{
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				delete children[i];
				children[i] = 0;
			}
			children.clear();
		}

		std::string name;
		unsigned int id;

		mat4 nodeTransform;
		mat4 boneOffset;

		std::vector<float> mTimes;

		std::vector<vec3> positions;
		std::vector<quat> rotations;
		std::vector<vec3> scalings;

		std::vector<sBone*> children;
	};

	std::string name = "";
	unsigned int numBones = 0;

	double duration = 0.f;
	double ticksPerSecond = 0.f;

	std::map<std::string, unsigned int> bones;

	mat4 GlobalInverse;
	sBone root;
};