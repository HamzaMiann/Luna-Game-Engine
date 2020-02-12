#pragma once

#include <iApplication.h>


typedef class cApplication : public iApplication
{
	static iApplication* app;
public:
	virtual ~cApplication() {}

	static iApplication* Instance()
	{
		static cApplication app;
		return &app;
	}

protected:
	cApplication() : iApplication() {}

	// Inherited via iApplication
	virtual void Init() override;

	virtual void Run() override;

	virtual void End() override;

} Application;