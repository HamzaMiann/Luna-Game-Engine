#pragma once

#include <glm/vec3.hpp>


class cIntegrator
{
public:

	void RK4(glm::vec3& x, glm::vec3& v, glm::vec3& a, glm::vec3& g, float gf, float dt)
	{

	}

	void Euler(glm::vec3& x, glm::vec3& v, glm::vec3& a, const glm::vec3& g, float gf, float dt)
	{
		v += a * dt;
		v += g * dt * gf;
		x += v * dt;
	}

};