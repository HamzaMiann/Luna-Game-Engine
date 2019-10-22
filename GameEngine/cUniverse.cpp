#include "cUniverse.h"
#include "Scene.h"
#include "cLightManager.h"
#include "Mathf.h"

static cUniverse* universe_instance;
cUniverse* cUniverse::Get_Instance()
{
	if (!universe_instance) universe_instance = new cUniverse();
	return universe_instance;
}

sNVPair cUniverse::ReceiveMessage(sNVPair message)
{
	if (message.name == "get closest planet")
	{
		float closest = FLT_MAX;
		glm::vec3* closestPtr = nullptr;

		// loop through each planet to find the closest planet
		for (int i = 0; i < planets.size(); ++i)
		{
			for (int k = 0; k < planets[i].size(); ++k)
			{
				// if energy is less than 1, assume 0
				float r = planets[i][k]->ReceiveMessage(sNVPair("get_energy", 0.f)).fValue;
				if (r < 1.f) continue;

				float distance = glm::distance(planets[i][k]->positionXYZ, message.v3Value);
				if (distance < closest)
				{
					closest = distance;
					closestPtr = &planets[i][k]->positionXYZ;
				}
			}
		}
		message.v3Ptr = closestPtr;
	}
	else if (message.name == "harvest energy from planet")
	{
		// get the object by pointer
		cSpaceObject* planet = planetsByPosition[message.v3Ptr];
		float resources = planet->ReceiveMessage(sNVPair("get_energy", 0.f)).fValue;
		printf("Ship taking resource from planet. %f left.\n", resources);

		if (resources > 0.f)
		{
			planet->ReceiveMessage(sNVPair("remove_energy", .5f));
			message.fValue = .5f;
			message.name = "add energy";
		}
		else
		{
			message.name = "empty";
		}
	}
	else if (message.name == "get closest enemy")
	{
		message.v3Ptr = shipPairs[message.v3Ptr];
	}
	else if (message.name == "attack enemy")
	{
		message.v3Ptr = shipPairs[message.v3Ptr];
		for (unsigned int i = 0; i < ships.size(); ++i)
		{
			if (&ships[i]->positionXYZ == message.v3Ptr)
			{
				if (ships[i]->ReceiveMessage(sNVPair("take damage", message.fValue)).name == "dead")
				{
					shipPairs.erase(shipPairs[message.v3Ptr]);
					shipPairs.erase(message.v3Ptr);
					ships[i]->meshName = "";
					ships.erase(ships.begin() + i);
					break;
				}
			}
		}
	}
	else if (message.name == "attack all")
	{
		for (unsigned int i = 0; i < ships.size(); ++i)
		{
			if (ships[i]->ReceiveMessage(sNVPair("take damage", 10.f)).name == "dead")
			{
				shipPairs.erase(shipPairs[message.v3Ptr]);
				shipPairs.erase(message.v3Ptr);
				ships[i]->meshName = "";
				ships.erase(ships.begin() + i);
				break;
			}
		}
	}
	return message;
}

void cUniverse::Instantiate(Scene* scene, std::size_t number_of_stars, std::size_t number_of_planets)
{
	// the first given light is the template light
	sLight* originalLight = scene->pLightManager->Lights[0];
	for (unsigned int i = 0; i < number_of_stars; ++i)
	{
		cSpaceObject* star = this->factory.make_object("star");
		bool tooClose = true;
		while (tooClose)
		{
			star->positionXYZ = glm::vec3(rand() % (20) - 10,
										  rand() % (20) - 10,
										  rand() % (20) - 10);
			tooClose = false;
			for (unsigned int n = 0; n < stars.size(); ++n)
			{
				if (glm::distance(star->positionXYZ, stars[n]->positionXYZ) < 7.f)
				{
					tooClose = true;
				}
			}
		}
		
		stars.push_back(star);
		scene->vecGameObjects.push_back(star);

		sLight* light = new sLight;
		light->position = glm::vec4(star->positionXYZ, 1.f);
		light->atten = originalLight->atten;
		light->diffuse = originalLight->diffuse;
		light->specular = originalLight->specular;
		light->direction = originalLight->direction;
		light->param1 = originalLight->param1;
		light->param2 = originalLight->param2;
		
		scene->pLightManager->Lights.push_back(light);

		planets.push_back(std::vector<cSpaceObject*>());
		for (unsigned int k = 0; k < number_of_planets; ++k)
		{
			cSpaceObject* planet = this->factory.make_object("planet");
			planet->positionXYZ = glm::vec3(star->positionXYZ.x + rand() % 2 + (k + 2),
											star->positionXYZ.y,
											star->positionXYZ.z + rand() % 2 + (k + 2));
			planets[i].push_back(planet);
			scene->vecGameObjects.push_back(planet);

			planetsByPosition[&planet->positionXYZ] = planet;

			if (rand() % 2 == 1) break;
		}
	}

	cSpaceObject* ship = this->factory.make_object("ship");
	this->factory.build_details(ship, "basic_ship");
	ships.push_back(ship);
	scene->vecGameObjects.push_back(ship);

	ship = this->factory.make_object("ship");
	this->factory.build_details(ship, "green_ship");
	ships.push_back(ship);
	scene->vecGameObjects.push_back(ship);

	ship = this->factory.make_object("ship");
	this->factory.build_details(ship, "basic_ship");
	ships.push_back(ship);
	scene->vecGameObjects.push_back(ship);

	// erase the first light which was added as a template
	scene->pLightManager->Lights.erase(scene->pLightManager->Lights.begin());
}


void cUniverse::Update(float delta_time)
{
	// loop through all stars
	for (unsigned int i = 0; i < stars.size(); ++i)
	{
		cSpaceObject* star = stars[i];

		// loop through each planet of each star
		for (unsigned int k = 0; k < planets[i].size(); ++k)
		{
			// rotate the planet around the star
			cSpaceObject* planet = planets[i][k];
			Mathf::rotate_vector(delta_time * planet->ReceiveMessage(sNVPair("get_speed", 0.f)).fValue,
								 star->positionXYZ,
								 planet->positionXYZ);
		}
	}
	
	// refresh the ship pairs
	shipPairs.clear();

	// loop through all ships
	for (unsigned int n = 0; n < ships.size(); ++n)
	{
		// Run through all other ships to find the closest ship
		//--------------------------------------
		glm::vec3* ptr = shipPairs[&ships[n]->positionXYZ];
		if (!ptr)
		{
			float distance = FLT_MAX;
			glm::vec3* closest = nullptr;
			
			for (unsigned int m = 0; m < ships.size(); ++m)
			{
				if (m == n) continue;

				float d = glm::distance(ships[n]->positionXYZ, ships[n]->positionXYZ);
				if (d < distance)
				{
					distance = d;
					closest = &ships[m]->positionXYZ;
				}
			}

			// if a closest ship was found, store it as a pair
			if (closest)
			{
				shipPairs[&ships[n]->positionXYZ] = closest;
				shipPairs[closest] = &ships[n]->positionXYZ;
			}
		}
		//--------------------------------------

		unsigned int current_size = ships.size();

		// update ship logic
		ships[n]->Update();

		if (ships.size() < current_size) --n;

	}
}
