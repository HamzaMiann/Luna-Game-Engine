#pragma once

#include <string>

class iMessageInterface
{
public:
	virtual ~iMessageInterface() {}
	virtual void ReceiveMessage(std::string message) = 0;
};