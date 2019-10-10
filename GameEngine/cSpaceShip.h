#pragma once

#include "cGameObject.h"
#include "iMessageInterface.h"
#include "cDebugRenderer.h"

class cSpaceShip : public cGameObject, public iMessageInterface
{
private:
	iMessageInterface* pWorld;
	glm::vec3* target = nullptr;
public:
	cSpaceShip()
	{
		this->inverseMass = 0.f;
		this->meshName = "space_ship";
		//this->uniformColour = true;
		
		this->objectColourRGBA = glm::vec4(.5f);
		this->scale = .2f;
	}
	void Set_World(iMessageInterface* interface)
	{
		this->pWorld = interface;
	}
	virtual sNVPair ReceiveMessage(sNVPair message) override { return sNVPair(); }
	virtual void Update();
	
};