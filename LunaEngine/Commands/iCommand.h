#pragma once


class iCommand
{
public:
	virtual ~iCommand() {}
	virtual void Update(float delta_time) = 0;
	virtual bool Is_Done() = 0;
};