#pragma once

#include <vector>
#include <string>
#include <glm/gtx/quaternion.hpp>

class cAnimation
{
	typedef glm::quat quat;
	typedef glm::vec3 vec3;
	typedef glm::mat4 mat4;
public:

	struct sAnimationNode
	{
		std::string name;
		std::vector<mat4> rotatedPositions;
		std::vector<vec3> scalings;
	};

	std::string name;
	double duration;
	double ticksPerSecond;
	std::vector<sAnimationNode> channels;
};