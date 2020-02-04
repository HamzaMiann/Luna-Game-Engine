#pragma once

#include <string>

class Shader
{
	int _id;
	std::string _name;

public:
	~Shader() {}
	Shader() :
		_id(-1),
		_name("")
	{
	}
	Shader(int ID) :
		_id(ID),
		_name("")
	{
	}
	Shader(std::string name);

	void SetShader(int ID);
	void SetShader(std::string name);

	inline int GetID() { return _id; }
	inline std::string GetName() { return _name; }

};