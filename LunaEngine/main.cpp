
#include <iApplication.h>

int main(void)
{
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

	return 0;
}