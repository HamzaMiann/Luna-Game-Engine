#pragma once

#include <vector>
#include "sLight.h"

class cLightManager
{
private:
	cLightManager() {}
public:
	std::vector<sLight*> Lights;
	void Set_Light_Data(int shader_id);

	static cLightManager* Instance()
	{
		static cLightManager instance;
		return &instance;
	}
};