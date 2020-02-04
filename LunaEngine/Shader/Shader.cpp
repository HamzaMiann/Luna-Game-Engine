#include "Shader.h"
#include <Shader/cShaderManager.h>

Shader::Shader(std::string name):
	_id(-1),
	_name(name)
{
	_id = cShaderManager::Instance()->getIDFromFriendlyName(name);
}

void Shader::SetShader(int ID)
{
	_id = ID;
	_name = "";
}

void Shader::SetShader(std::string name)
{
	_name = name;
	_id = cShaderManager::Instance()->getIDFromFriendlyName(name);
}

