#pragma once

#include "cGameObject.h"
#include "iMessageInterface.h"

class cSpaceObject : public cGameObject, public iMessageInterface
{
public:
	virtual ~cSpaceObject() {}
	virtual sNVPair ReceiveMessage(sNVPair message) = 0;
	virtual void Update() {};
};