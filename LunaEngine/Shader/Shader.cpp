#include "Shader.h"
#include <Shader/cShaderManager.h>
#include <_GL/GLCommon.h>

int Shader::last_id = -1;

Shader::Shader(std::string name):
	_id(-1),
	_name(name)
{
	_id = cShaderManager::Instance()->getIDFromFriendlyName(name);
	if (_id > 0)
		CacheUniforms();
}

void Shader::SetShader(int ID)
{
	_id = ID;
	_name = "";
	if (_id > 0)
		CacheUniforms();
}

void Shader::SetShader(std::string name)
{
	_name = name;
	_id = cShaderManager::Instance()->getIDFromFriendlyName(name);
	if (_id > 0)
		CacheUniforms();
}

void Shader::CacheUniforms()
{
	int num_uniforms;
	glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &num_uniforms);

	const GLsizei bufSize = 32;

	for (int i = 0; i < num_uniforms; ++i)
	{
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length
		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		glGetActiveUniform(_id, (GLuint)i, bufSize, &length, &size, &type, name);

		GLint loc = glGetUniformLocation(_id, name);

		uniforms[name] = Uniform(name, loc);
	}
}

Shader* Shader::FromName(std::string name)
{
	return cShaderManager::Instance()->GetShader(name);
}

