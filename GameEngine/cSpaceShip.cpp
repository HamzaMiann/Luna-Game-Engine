#include "cSpaceShip.h"


sNVPair cSpaceShip::ReceiveMessage(sNVPair message)
{
	sNVPair returnMessage("ok", 0.f);
	if (pLogic)
		return pLogic->ReceiveMessage(message);
	return returnMessage;
}

void cSpaceShip::Update()
{
	if (pLogic)
		pLogic->Update(this->pWorld, this);
}
