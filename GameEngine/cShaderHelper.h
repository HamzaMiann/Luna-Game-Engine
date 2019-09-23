#pragma once

#include <string>

class cShaderHelper
{
public:
	cShaderHelper() = delete;
	static unsigned int BuildShaderProgram(std::string vertexShaderFile, std::string fragShaderFile);
};