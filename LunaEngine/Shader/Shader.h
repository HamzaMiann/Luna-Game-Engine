#pragma once

#include <string>
#include <map>

class Shader
{
	struct Uniform
	{
		Uniform():
			name(""),
			ID(-1)
		{ }

		Uniform(std::string name, int id) :
			name(name),
			ID(id)
		{
		}

		std::string name;
		int ID;
	};
	int _id;
	std::string _name;
	std::map<std::string, Uniform> uniforms;

public:

	static int last_id;
	static inline void ResetLast() { last_id = -1; }

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
		CacheUniforms();
	}
	Shader(std::string name);

	void SetShader(int ID);
	void SetShader(std::string name);

	void CacheUniforms();

	inline int GetID() { return _id; }
	inline std::string GetName() { return _name; }

	inline int operator[](std::string uniform) { return uniforms[uniform].ID; }

};