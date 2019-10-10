#pragma once

#include "iMessageInterface.h"
#include "cGameObject.h"

class cPlanet : public cGameObject, public iMessageInterface
{
public:
	float ResourcesLeft = 100.f;
	float RotationSpeed;

	cPlanet()
	{
		this->meshName = "sphere";
		this->scale = (rand() % 10) / 10.f;
		this->objectColourRGBA = glm::vec4(.1f, .3f, .9f, 1.f);
		this->inverseMass = 0.f;
		this->RotationSpeed = rand() % 10 + 10;
	}
	virtual sNVPair ReceiveMessage(sNVPair message) override { return sNVPair(); }
};