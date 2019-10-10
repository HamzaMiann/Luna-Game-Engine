#pragma once

#include "sNVPair.h"

class iMessageInterface
{
public:
	virtual ~iMessageInterface() {}
	virtual sNVPair ReceiveMessage(sNVPair message) = 0;
};