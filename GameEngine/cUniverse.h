#pragma once

#include "iMessageInterface.h"
#include "cStar.h"
#include "cPlanet.h"
#include "cSpaceShip.h"
#include <vector>

class Scene;

class cUniverse : public iMessageInterface
{
private:
	std::vector<cStar*> stars;
	std::vector<std::vector<cPlanet*>> planets;
	std::vector<cSpaceShip*> ships;

public:
	cUniverse() {};
	
	virtual sNVPair ReceiveMessage(sNVPair message) override;
	

	void Instantiate(Scene* scene, std::size_t number_of_stars, std::size_t number_of_planets);
	void Update(float delta_time);

};