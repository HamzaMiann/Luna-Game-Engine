#pragma once

#include <string>
#include <map>
#include <Texture/cTexture.h>
#include <_GL/GLCommon.h>
#include <glm/glm_common.h>


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

	inline void Use() { glUseProgram(_id); }

	inline void SetFloat(const std::string& uniform, float value)
	{
		glUniform1f((*this)[uniform], (float)value);
	}

	inline void SetBool(const std::string& uniform, bool value)
	{
		glUniform1f((*this)[uniform], (float)value);
	}

	inline void SetVec2(const std::string& uniform, const vec2& vector)
	{
		glUniform2f((*this)[uniform], vector.x, vector.y);
	}

	inline void SetVec3(const std::string& uniform, const vec3& vector)
	{
		glUniform3f((*this)[uniform], vector.x, vector.y, vector.z);
	}

	inline void SetVec4(const std::string& uniform, const vec4& vector)
	{
		glUniform4f((*this)[uniform], vector.x, vector.y, vector.z, vector.w);
	}

	inline void SetMat4(const std::string& uniform, const mat4& matrix)
	{
		glUniformMatrix4fv((*this)[uniform], 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void SetTexture(cTexture& texture, const std::string& uniform, int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture.GetID());
		glUniform1i((*this)[uniform], textureUnit);
	}

	void SetTexture(int textureID, const std::string& uniform, int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i((*this)[uniform], textureUnit);
	}

	void SetShader(int ID);
	void SetShader(std::string name);

	void CacheUniforms();

	inline int GetID() { return _id; }
	inline std::string GetName() { return _name; }

	inline int operator[](std::string uniform) { return uniforms[uniform].ID; }

	static Shader* FromName(std::string name);

};