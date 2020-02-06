#pragma once

#include <vector>
#include <Shader/Shader.h>
#include "sLight.h"

class cLightManager
{
private:
	cLightManager() {}
public:
	std::vector<sLight*> Lights;
	//void Set_Light_Data(int shader_id);
	void Set_Light_Data(Shader& shader);

	static cLightManager* Instance()
	{
		static cLightManager instance;
		return &instance;
	}
};