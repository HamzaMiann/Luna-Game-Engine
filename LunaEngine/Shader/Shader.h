#pragma once

#include <string>
#include <vector>
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

	void CacheUniforms();

public:

	static int last_id;
	static inline void ResetLast() { last_id = -1; }

	~Shader() {}

	/*
	Uninitialized shader
	*/
	Shader() :
		_id(-1),
		_name("")
	{
	}

	/*
	Initializes the shader with the given ID
	*/
	Shader(int ID) :
		_id(ID),
		_name("")
	{
		CacheUniforms();
	}

	/*
	Initializes the shader with the given shader friendly name
	*/
	Shader(std::string name);

	/*
	Uses this shader program (glUseProgram)
	*/
	inline void Use() { glUseProgram(_id); }

	/*
	Sets 1f float value for the given uniform
	*/
	inline void SetFloat(const std::string& uniform, float value)
	{
		glUniform1f((*this)[uniform], (float)value);
	}

	/*
	Sets 1f bool value for the given uniform
	*/
	inline void SetBool(const std::string& uniform, bool value)
	{
		glUniform1f((*this)[uniform], (float)value);
	}

	/*
	Sets 2f vec2 value for the given uniform
	*/
	inline void SetVec2(const std::string& uniform, const vec2& vector)
	{
		glUniform2f((*this)[uniform], vector.x, vector.y);
	}

	/*
	Sets 3f vec3 value for the given uniform
	*/
	inline void SetVec3(const std::string& uniform, const vec3& vector)
	{
		glUniform3f((*this)[uniform], vector.x, vector.y, vector.z);
	}

	/*
	Sets 4f vec4 value for the given uniform
	*/
	inline void SetVec4(const std::string& uniform, const vec4& vector)
	{
		glUniform4f((*this)[uniform], vector.x, vector.y, vector.z, vector.w);
	}

	/*
	Sets 4fv mat4 value for the given uniform
	*/
	inline void SetMat4(const std::string& uniform, const mat4& matrix)
	{
		glUniformMatrix4fv((*this)[uniform], 1, GL_FALSE, glm::value_ptr(matrix));
	}

	/*
	Sets 4fv mat4 array for the given uniform
	*/
	inline void SetMat4Array(const std::string& uniform, const std::vector<mat4>& matrices)
	{
		glUniformMatrix4fv(glGetUniformLocation(_id, uniform.c_str()), (GLint)matrices.size(), GL_FALSE, glm::value_ptr(matrices[0]));
	}

	/*
	Binds the texture to the given texture unit and uniform
	(Texture unit must be unique for each texture that is set)
	*/
	void SetTexture(cTexture& texture, const std::string& uniform, int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, texture.GetID());
		glUniform1i((*this)[uniform], textureUnit);
	}

	/*
	Binds the texture ID to the given texture unit and uniform
	(Texture unit must be unique for each texture that is set)
	*/
	void SetTexture(int textureID, const std::string& uniform, int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i((*this)[uniform], textureUnit);
	}

	void SetTexture3D(int textureID, const std::string& uniform, int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i((*this)[uniform], textureUnit);
	}

	void SetTexture3D(cTexture& texture, const std::string& uniform, int textureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_3D, texture.GetID());
		glUniform1i((*this)[uniform], textureUnit);
	}

	/*
	Binds the cubemap to texture unit number 26 for the given uniform
	(Texture unit must be unique for each texture that is set)
	*/
	void SetCubemap(const std::string& uniform, int textureID)
	{
		glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
		glUniform1i((*this)[uniform], 26);
	}

	/*
	Sets the shader by the given ID.
	(This is an expensive call as it will cache all uniforms every time it is called)
	(Use the FromShader() call instead for faster performance)
	*/
	void SetShader(int ID);
	
	/*
	Sets the shader by the given name.
	(This is an expensive call as it will cache all uniforms every time it is called)
	(Use the FromShader() call instead for faster performance)
	*/
	void SetShader(std::string name);

	/*
	Returns the ID of the shader
	*/
	inline int GetID() { return _id; }

	/*
	Returns the name of the shader
	*/
	inline std::string GetName() { return _name; }

	inline int operator[](const std::string& uniform) { return uniforms[uniform].ID; }

	/*
	Gets the shader by the given name. Will return nullptr if the shader does not exist.
	*/
	static Shader* FromName(std::string name);

};