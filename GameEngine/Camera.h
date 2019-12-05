#pragma once
#include <glm/vec3.hpp>

class Camera
{
	typedef glm::vec3 vec3;
public:
	vec3	Eye = vec3(0.0f, 0.0f, -10.0);
	vec3	Target = vec3(0.0f, 0.0f, 0.0f);
	vec3	Up = vec3(0.0f, 1.0f, 0.0f);
};