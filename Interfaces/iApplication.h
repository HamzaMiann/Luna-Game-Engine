#pragma once

#include <exception>
#include <iostream>

class iApplication
{
	friend int main(void);
	friend int main();

	static iApplication* _instance;
public:
	virtual ~iApplication() {}

	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void End() = 0;

protected:
	iApplication()
	{
		if (_instance != nullptr)
		{
			std::cout << "===========================================================" << std::endl;
			std::cout << "Multiple applications registered to game engine. Exiting..." << std::endl;
			std::cout << "===========================================================" << std::endl;
			exit(1);
		}
		_instance = this;
	}
};