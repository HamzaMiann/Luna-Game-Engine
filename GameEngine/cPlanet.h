#pragma once

#include "cSpaceObject.h"

class cPlanet : public cSpaceObject
{
public:
	float ResourcesLeft = 100.f;
	float RotationSpeed;

	cPlanet()
	{
		this->meshName = "sphere";
		this->scale = (rand() % 10 + 1) / 10.f;
		this->objectColourRGBA = glm::vec4(.1f, .3f, .9f, 1.f);
		this->inverseMass = 0.f;
		this->RotationSpeed = rand() % 10 + 10;
		this->specularStrength = 50.f;
	}
	virtual sNVPair ReceiveMessage(sNVPair message) override
	{
		sNVPair returnMessage;
		returnMessage.name = "ok";
		if (message.name == "get_energy")
		{
			returnMessage.fValue = ResourcesLeft;
		}
		else if (message.name == "remove_energy")
		{
			if (this->ResourcesLeft > 0.f)
			{
				ResourcesLeft -= message.fValue;
				this->objectColourRGBA.b *= .99f;
				this->objectColourRGBA.r *= 1.01f;
			}
		}
		else if (message.name == "get_speed")
		{
			returnMessage.fValue = RotationSpeed;
		}
		return returnMessage;
	}
};