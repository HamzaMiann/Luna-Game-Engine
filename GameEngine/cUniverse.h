#pragma once

#include "iMessageInterface.h"
#include "cStar.h"
#include "cPlanet.h"
#include <vector>

class Scene;

class cUniverse : public iMessageInterface
{
private:
	std::vector<cStar*> stars;
	std::vector<std::vector<cPlanet*>> planets;

public:
	cUniverse() {};
	
	virtual void ReceiveMessage(std::string message) override {}

	void Instantiate(Scene* scene, std::size_t number_of_stars, std::size_t number_of_planets);
	void Update(float delta_time);
};