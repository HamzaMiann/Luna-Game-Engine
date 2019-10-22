
#include "cUniverse.h"
#include "cSpaceFactory.h"
#include "cPlanet.h"
#include "cStar.h"
#include "cSpaceShip.h"
#include "cSpaceShipImplementation.h"

cSpaceObject * cSpaceFactory::make_object(std::string type)
{
	if (type == "planet")
	{
		return new cPlanet();
	}
	if (type == "star")
	{
		return new cStar();
	}
	if (type == "ship")
	{
		return new cSpaceShip();
	}
	if (type == "box")
	{
		return new cBox();
	}
	return nullptr;
}

bool cSpaceFactory::build_details(cSpaceObject * object, std::string type)
{
	if (type == "shiny")
	{
		object->specularStrength = 100.f;
		return true;
	}
	if (type == "basic_ship")
	{
		object->positionXYZ = glm::vec3(
			rand() % 30 - 15,
			rand() % 30 - 15,
			rand() % 30 - 15
		);
		object->previousXYZ = object->positionXYZ;
		object->specularStrength = 100.f;
		object->objectColourRGBA = glm::vec4(.5f);
		object->scale = .2f;
		((cSpaceShip*)object)->Set_Implementation(new cSpaceShipImplementation());
		((cSpaceShip*)object)->Set_World(cUniverse::Get_Instance());
		return true;
	}
	if (type == "green_ship")
	{
		object->positionXYZ = glm::vec3(
			rand() % 30 - 15,
			rand() % 30 - 15,
			rand() % 30 - 15
		);
		object->previousXYZ = object->positionXYZ;
		object->specularStrength = 50.f;
		object->objectColourRGBA = glm::vec4(.1f, 1.f, .1f, 1.f);
		object->scale = .25f;
		((cSpaceShip*)object)->Set_Implementation(new cSpaceShipImplementation());
		((cSpaceShip*)object)->Set_World(cUniverse::Get_Instance());
		return true;
	}
	if (type == "nothing_ship")
	{
		object->positionXYZ = glm::vec3(
			rand() % 30 - 15,
			rand() % 30 - 15,
			rand() % 30 - 15
		);
		object->previousXYZ = object->positionXYZ;
		object->specularStrength = 50.f;
		object->objectColourRGBA = glm::vec4(.1f, 1.f, .1f, 1.f);
		object->scale = .25f;
		((cSpaceShip*)object)->Set_Implementation(new cDoNothingImpl());
		((cSpaceShip*)object)->Set_World(cUniverse::Get_Instance());
		return true;
	}
	return false;
}
