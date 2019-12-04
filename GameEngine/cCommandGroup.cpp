
#include "cCommandGroup.h"

cCommandGroup::~cCommandGroup()
{

}

void cCommandGroup::Update(float delta_time)
{
	// Serial commands
	if (Serial.size() > 0)
	{
		iCommand* cmd = *Serial.begin();
		cmd->Update(delta_time);
		if (cmd->Is_Done())
		{
			Serial.erase(Serial.begin());
			delete cmd;
		}
	}

	// Parallel
	for (int i = 0; i < Parallel.size(); ++i)
	{
		iCommand* cmd = *(Parallel.begin() + i);
		cmd->Update(delta_time);
		if (cmd->Is_Done())
		{
			delete cmd;
			Parallel.erase(Parallel.begin() + i);
			--i;
		}
	}
}

bool cCommandGroup::Is_Done()
{
	return (Serial.size() == 0 && Parallel.size() == 0);
}

void cCommandGroup::AddParallel(iCommand* command)
{
	Parallel.push_back(command);
}

void cCommandGroup::AddSerial(iCommand* command)
{
	Serial.push_back(command);
}
