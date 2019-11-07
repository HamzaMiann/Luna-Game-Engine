#pragma once

#include <vector>
#include <glm/vec3.hpp>

struct cParticle
{
	glm::vec3 pos;
	glm::vec3 vel;
	float life_time = -1.f;
};

class cParticleEffect
{
public:
	cParticleEffect(unsigned int numParticles) {}
	~cParticleEffect() {}

	void Generate();

	void Step(float delta_time);
};