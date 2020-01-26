#pragma once

#include <Components/iComponent.h>

class iBehaviour : public iComponent
{
public:
	iBehaviour();
	virtual ~iBehaviour();
	virtual void start() = 0;
	virtual void update(float dt) = 0;
};