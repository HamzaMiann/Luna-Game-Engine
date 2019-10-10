#include "cUniverse.h"
#include "Scene.h"
#include "cLightManager.h"
#include "Mathf.h"

sNVPair cUniverse::ReceiveMessage(sNVPair message)
{
	if (message.name == "get closest planet")
	{
		float closest = FLT_MAX;
		glm::vec3* closestPtr = nullptr;
		for (int i = 0; i < planets.size(); ++i)
		{
			for (int k = 0; k < planets[i].size(); ++k)
			{
				float r = planets[i][k]->ResourcesLeft;
				if (r < 1.f) continue;

				float distance = glm::distance2(planets[i][k]->positionXYZ, message.v3Value);
				if (distance < closest);
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
		for (int i = 0; i < planets.size(); ++i)
		{
			for (int k = 0; k < planets[i].size(); ++k)
			{
				if (&planets[i][k]->positionXYZ == message.v3Ptr)
				{
					printf("Ship taking resource from planet. %f left.\n", planets[i][k]->ResourcesLeft);
					if (planets[i][k]->ResourcesLeft > 0.f)
					{
						planets[i][k]->ResourcesLeft -= 1.f;
						message.fValue = 1.f;
						message.name = "add energy";
					}
					else
					{
						message.name = "empty";
					}
					break;
				}
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
		cStar* star = new cStar;
		bool tooClose = true;
		while (tooClose)
		{
			//srand((unsigned int)((float)glfwGetTime() * 100.f + i));
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
		
		//star->positionXYZ = glm::vec3(0.1f);
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

		planets.push_back(std::vector<cPlanet*>());
		for (unsigned int k = 0; k < number_of_planets; ++k)
		{
			cPlanet* planet = new cPlanet;
			planet->positionXYZ = glm::vec3(star->positionXYZ.x + rand() % 2 + (k + 2),
											star->positionXYZ.y,
											star->positionXYZ.z + rand() % 2 + (k + 2));
			planets[i].push_back(planet);
			scene->vecGameObjects.push_back(planet);

			if (rand() % 2 == 1) break;
		}
	}

	cSpaceShip* ship = new cSpaceShip;
	ship->Set_World(this);
	ships.push_back(ship);
	scene->vecGameObjects.push_back(ship);

	// erase the first light which was added as a template
	scene->pLightManager->Lights.erase(scene->pLightManager->Lights.begin());
}

void cUniverse::Update(float delta_time)
{
	for (unsigned int i = 0; i < stars.size(); ++i)
	{
		cStar* star = stars[i];
		for (unsigned int k = 0; k < planets[i].size(); ++k)
		{
			cPlanet* ptr = planets[i][k];
			ptr->positionXYZ -= star->positionXYZ;
			Mathf::rotate_vector(delta_time * ptr->RotationSpeed,
								 glm::vec3(0.f),
								 ptr->positionXYZ);
			ptr->positionXYZ += star->positionXYZ;
			//ptr->positionXYZ.x += delta_time;
		}
	}
	for (unsigned int n = 0; n < ships.size(); ++n)
	{
		ships[n]->Update();
	}
}
