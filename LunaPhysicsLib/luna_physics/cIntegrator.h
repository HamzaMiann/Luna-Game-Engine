#pragma once

#include <glm/vec3.hpp>


class cIntegrator
{
public:

	void RK4(glm::vec3& x, glm::vec3& v, glm::vec3& a, glm::vec3& g, float dt)
	{

	}

	void Euler(glm::vec3& x, glm::vec3& v, glm::vec3& a, glm::vec3& g, float dt)
	{
		v += a * dt;
		v += g * dt;
		x += v * dt;
	}

};