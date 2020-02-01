#pragma once

#include <glm/vec3.hpp>


class cIntegrator
{
public:

	/*
	x	= position
	v	= velocity
	a	= accelteration
	g	= gravitational acceleration
	gf	= gravitational impact ( 0 - INFINITE )
	dt	= delta time
	t	= last frame time
	*/
	void RK4(glm::vec3& x, glm::vec3& v, glm::vec3& a, glm::vec3& g, float gf, float dt, float t);


	/*
	x	= position
	v	= velocity
	a	= accelteration
	g	= gravitational acceleration
	gf	= gravitational impact ( 0 - INFINITE )
	dt	= delta time
	*/
	void Euler(glm::vec3& x, glm::vec3& v, glm::vec3& a, const glm::vec3& g, float gf, float dt);

};