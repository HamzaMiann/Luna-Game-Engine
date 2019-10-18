#pragma once

#include "cSpaceObject.h"
#include "iImplementation.h"

class cSpaceShip : public cSpaceObject
{
private:
	iMessageInterface* pWorld;
	iImplementation* pLogic;

public:

	cSpaceShip()
	{
		this->meshName = "space_ship";
		this->inverseMass = 0.f;
	}

	void Set_World(iMessageInterface* interface)
	{
		this->pWorld = interface;
	}

	iMessageInterface* Get_World()
	{
		return this->pWorld;
	}

	void Set_Implementation(iImplementation* impl)
	{
		pLogic = impl;
	}

	virtual sNVPair ReceiveMessage(sNVPair message) override;
	virtual void Update();
	
};