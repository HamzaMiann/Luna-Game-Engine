
#include <iApplication.h>
#include <Steam/SteamModule.h>

int main(void)
{
	// Initialize Steam
	InitSteam();

	if (iApplication::_instance)
	{
		iApplication::_instance->Init();
		iApplication::_instance->Run();
		iApplication::_instance->End();
	}
	else
	{
		std::cout << "No application found..." << std::endl;
	}

	// Release Steam
	ReleaseSteam();

	return 0;
}