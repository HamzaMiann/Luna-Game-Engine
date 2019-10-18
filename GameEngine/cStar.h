#pragma once

#include "cSpaceObject.h"

class cStar : public cSpaceObject
{
public:
	cStar()
	{
		this->scale = 2.f;
		this->meshName = "sphere";
		this->uniformColour = true;
		this->objectColourRGBA = glm::vec4(.9f, .7f, .0f, 1.f);
		this->inverseMass = 0.f;
	}
	virtual sNVPair ReceiveMessage(sNVPair message) override { return sNVPair(); }
};