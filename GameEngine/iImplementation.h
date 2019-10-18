#pragma once

#include "iMessageInterface.h"
#include "cGameObject.h"

class iImplementation : public iMessageInterface
{
public:
	virtual ~iImplementation() {}
	virtual sNVPair ReceiveMessage(sNVPair message) = 0;
	virtual void Update(iMessageInterface* mediator, cGameObject* self) = 0;
};