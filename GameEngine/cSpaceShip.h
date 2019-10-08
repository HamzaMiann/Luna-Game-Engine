#pragma once

#include "cGameObject.h"
#include "iMessageInterface.h"

class cSpaceShip : public cGameObject, public iMessageInterface
{
private:
	iMessageInterface* pWorld;
public:
	cSpaceShip()
	{
		this->inverseMass = 0.f;
		this->meshName = "space_ship";
	}
	void Set_World(iMessageInterface* interface)
	{
		this->pWorld = interface;
	}
	virtual void ReceiveMessage(std::string message) override {}
};