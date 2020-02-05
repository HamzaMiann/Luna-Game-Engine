#pragma once


#include <_GL/GLCommon.h>

class global
{
public:
	static GLFWwindow* window;
};

void SetErrorCallback(GLFWerrorfun callback);
void SetKeyCallback(GLFWkeyfun callback);
void InitGL();
