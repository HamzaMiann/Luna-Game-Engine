
#include "cShaderBuilder.h"
#include "cShaderHelper.h"
#include <iostream>
using namespace rapidxml;

void cShaderBuilder::Build(Scene& scene, xml_node<>* node)
{
	std::string vertShaderFile = node->first_node("Vertex")->first_attribute("file")->value();
	std::string fragShaderFile = node->first_node("Fragment")->first_attribute("file")->value();
	try
	{
		scene.shaderProgID = cShaderHelper::BuildShaderProgram(vertShaderFile, fragShaderFile);
	}
	catch (std::exception& err)
	{
		std::cout << "Error: " << err.what() << std::endl;
		exit(1);
	}
}