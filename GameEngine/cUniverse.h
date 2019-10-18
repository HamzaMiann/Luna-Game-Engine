#pragma once

#include "cSpaceFactory.h"
#include <vector>
#include <map>

class Scene;

class cUniverse : public iMessageInterface
{
private:
	cSpaceFactory factory;

	std::vector<cSpaceObject*> stars;
	std::vector<std::vector<cSpaceObject*>> planets;
	std::vector<cSpaceObject*> ships;

	std::map<glm::vec3*, cSpaceObject*> planetsByPosition;

	std::map<glm::vec3*, glm::vec3*> shipPairs;

	cUniverse() {};


public:

	~cUniverse() {}

	static cUniverse* Get_Instance();
	
	virtual sNVPair ReceiveMessage(sNVPair message) override;
	

	void Instantiate(Scene* scene, std::size_t number_of_stars, std::size_t number_of_planets);
	void Update(float delta_time);

};