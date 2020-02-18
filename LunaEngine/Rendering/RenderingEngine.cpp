#include "RenderingEngine.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <_GL/GLCommon.h>
#include <Shader/cShaderManager.h>
#include <Texture/cBasicTextureManager.h>
#include <Lighting/cLightManager.h>
#include <Mesh\cVAOManager.h>


RenderingEngine::RenderingEngine()
{
	width = 600;
	height = 800;
	pass_id = 1;

	float ratio = width / (float)height;

	projection = glm::perspective(0.6f,		// FOV
								  ratio,			// Aspect ratio
								  0.1f,			// Near clipping plane
								  100'000.f);		// Far clipping plane

	view = glm::mat4(1.f);
}

RenderingEngine::~RenderingEngine()
{
}

void RenderingEngine::UpdateView()
{
	view = glm::lookAt(
		Camera::main_camera->Eye,
		Camera::main_camera->Target,
		Camera::main_camera->Up
	);
}

void RenderingEngine::SetUpTextureBindings(cMaterial& material)
{
	if (pass_id != 1) return;

	Shader& shader = *material.shader;
	cTexture* textures = material.texture;


	glActiveTexture(GL_TEXTURE0);						// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, textures[0].GetID());	// Texture now assoc with texture unit 0

	glActiveTexture(GL_TEXTURE1);						// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, textures[1].GetID());	// Texture now assoc with texture unit 1

	glActiveTexture(GL_TEXTURE2);						// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, textures[2].GetID());	// Texture now assoc with texture unit 2

	glActiveTexture(GL_TEXTURE3);						// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, textures[3].GetID());	// Texture now assoc with texture unit 3

	glUniform1i(shader["textSamp00"], 0);	// Texture unit 0

	glUniform1i(shader["textSamp01"], 1);	// Texture unit 1

	glUniform1i(shader["textSamp02"], 2);	// Texture unit 2

	glUniform1i(shader["textSamp03"], 3);	// Texture unit 3


	glUniform4f(shader["tex_0_3_ratio"],
		textures[0].GetBlend(),
		textures[1].GetBlend(),
		textures[2].GetBlend(),
		textures[3].GetBlend()
	);

	return;
}

void RenderingEngine::Render(iObject* object)
{
	cMaterial* material = object->GetComponent<cMaterial>();
	if (material != nullptr)
	{
		Render(*material);
	}
}

void RenderingEngine::Render(cMaterial& material)
{
	Shader& shader = *material.shader;
	sTransform& transform = material.transform;
	Camera& camera = *Camera::main_camera;
	cBasicTextureManager* textureManager = cBasicTextureManager::Instance();
	GLint shaderProgID = shader.GetID();

	GLint scopeUL = shader["isScope"];
	if (material.layer == "scope")
	{
		glUniform1i(scopeUL, (int)GL_TRUE);
	}
	else
	{
		glUniform1i(scopeUL, (int)GL_FALSE);
	}



	GLint bIsSkyBox_UL = shader["isSkybox"];
	GLint isReflection = shader["isReflection"];
	GLint isRefraction = shader["isRefraction"];

	// Set Skybox
	GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
	glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0
	glUniform1i(shader["skyBox"], 26);

	if (material.isSkybox)
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		//glCullFace(GL_FRONT_AND_BACK);
		glUniform1i(shader["isReflection"], (int)GL_FALSE);

		glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);
		glUniform1i(isReflection, (int)GL_FALSE);
		glUniform1i(isRefraction, (int)GL_FALSE);

	}
	else
	{
		if (material.layer == "reflect")
		{
			glCullFace(GL_BACK);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			glUniform1i(isReflection, (int)GL_TRUE);
			glUniform1i(isRefraction, (int)GL_FALSE);

		}
		else if (material.layer == "refract")
		{
			glCullFace(GL_BACK);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			glUniform1i(isReflection, (int)GL_FALSE);
			glUniform1i(isRefraction, (int)GL_TRUE);

		}
		else
		{
			glUniform1i(isReflection, (int)GL_FALSE);
			glUniform1i(isRefraction, (int)GL_FALSE);
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			// Don't draw back facing triangles (default)
			glCullFace(GL_BACK);
			SetUpTextureBindings(material);
		}
	}

	glm::mat4 m(1.f);


	// ******* TRANSLATION TRANSFORM *********
	glm::mat4 matTrans
		= glm::translate(glm::mat4(1.0f),
			glm::vec3(
				transform.pos.x,
				transform.pos.y,
				transform.pos.z)
		);
	m = m * matTrans;
	// ******* TRANSLATION TRANSFORM *********


	// ******* ROTATION TRANSFORM *********
	m = m * glm::mat4(transform.rotation);
	// ******* ROTATION TRANSFORM *********

	if (pass_id != 1) m = glm::mat4(1.f);

	GLint matModelIT_UL = shader["matModelInverTrans"];
	glm::mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	// ******* SCALE TRANSFORM *********
	/*glm::mat4 scale = glm::scale(glm::mat4(1.0f),
								 glm::vec3(objPtr->scale,
										   objPtr->scale,
										   objPtr->scale));
	m = m * scale;*/
	// ******* SCALE TRANSFORM *********




	GLint matModel_UL = shader["matModel"];
	glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(transform.ModelMatrix()));




	glUniform4f(shader["eyeLocation"],
		camera.Eye.x,
		camera.Eye.y,
		camera.Eye.z,
		1.0f
	);

	/*glUniform4f(shader["diffuseColour"],
		objPtr->colour.x,
		objPtr->colour.y,
		objPtr->colour.z,
		objPtr->colour.w
	);*/

	glUniform4f(shader["specularColour"],
		material.specColour.x,
		material.specColour.y,
		material.specColour.z,
		material.specColour.w
	);

	glUniform1i(shader["isUniform"], material.isUniformColour);


	if (pass_id != 1)
	{
		cLightManager::Instance()->Set_Light_Data(shader);
	}

	if (material.isWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	sModelDrawInfo& drawInfo = *material.drawInfo;
	glBindVertexArray(drawInfo.VAO_ID);
	glDrawElements(
		GL_TRIANGLES,
		drawInfo.numberOfIndices,
		GL_UNSIGNED_INT,
		0);
	glBindVertexArray(0);
}
