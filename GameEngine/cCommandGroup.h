#pragma once

#include "iCommand.h"
#include <vector>

class cCommandGroup : public iCommand
{
	std::vector<iCommand*> Serial;
	std::vector<iCommand*> Parallel;

public:
	virtual ~cCommandGroup();
	virtual void Update(float delta_time);
	virtual bool Is_Done();

	void AddParallel(iCommand* command);
	void AddSerial(iCommand* command);
};