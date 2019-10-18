#pragma once

#include "iImplementation.h"
#include "cGameObject.h"

class cSpaceShipImplementation : public iImplementation
{
private:
	glm::vec3* target = nullptr;
	float health = 100.f;
	float damage = 2.f;

public:
	virtual ~cSpaceShipImplementation() {}
	virtual sNVPair ReceiveMessage(sNVPair message);
	virtual void Update(iMessageInterface* mediator, cGameObject* self);
};