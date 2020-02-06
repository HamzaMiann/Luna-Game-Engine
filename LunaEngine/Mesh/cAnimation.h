#pragma once

#include <vector>
#include <string>
#include <glm/gtx/quaternion.hpp>

class cAnimation
{
	typedef glm::quat quat;
	typedef glm::vec3 vec3;
public:

	struct sAnimationNode
	{
		std::string name;
		std::vector<vec3> positions;
		std::vector<quat> rotations;
		std::vector<vec3> scalings;
	};

	std::string name;
	double duration;
	double ticksPerSecond;
	std::vector<sAnimationNode> channels;
};