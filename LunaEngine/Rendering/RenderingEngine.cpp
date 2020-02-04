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
	view = glm::lookAt(camera.Eye, camera.Target, camera.Up);
}

void RenderingEngine::SetUpTextureBindingsForObject(cMaterial* material, GLint shaderProgID)
{
	if (pass_id != 1) return;
	//// Tie the texture to the texture unit
	//GLuint texSamp0_UL = ::g_pTextureManager->getTextureIDFromName("Pizza.bmp");
	//glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	//glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0
	cBasicTextureManager* textureManager = cBasicTextureManager::Instance();

	// Tie the texture to the texture unit
	GLuint texSamp0_UL = textureManager->getTextureIDFromName(material->texture[0]);
	glActiveTexture(GL_TEXTURE0);				// Texture Unit 0
	glBindTexture(GL_TEXTURE_2D, texSamp0_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp1_UL = textureManager->getTextureIDFromName(material->texture[1]);
	glActiveTexture(GL_TEXTURE1);				// Texture Unit 1
	glBindTexture(GL_TEXTURE_2D, texSamp1_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp2_UL = textureManager->getTextureIDFromName(material->texture[2]);
	glActiveTexture(GL_TEXTURE2);				// Texture Unit 2
	glBindTexture(GL_TEXTURE_2D, texSamp2_UL);	// Texture now assoc with texture unit 0

	GLuint texSamp3_UL = textureManager->getTextureIDFromName(material->texture[3]);
	glActiveTexture(GL_TEXTURE3);				// Texture Unit 3
	glBindTexture(GL_TEXTURE_2D, texSamp3_UL);	// Texture now assoc with texture unit 0

	// Tie the texture units to the samplers in the shader
	GLint textSamp00_UL = glGetUniformLocation(shaderProgID, "textSamp00");
	glUniform1i(textSamp00_UL, 0);	// Texture unit 0

	GLint textSamp01_UL = glGetUniformLocation(shaderProgID, "textSamp01");
	glUniform1i(textSamp01_UL, 1);	// Texture unit 1

	GLint textSamp02_UL = glGetUniformLocation(shaderProgID, "textSamp02");
	glUniform1i(textSamp02_UL, 2);	// Texture unit 2

	GLint textSamp03_UL = glGetUniformLocation(shaderProgID, "textSamp03");
	glUniform1i(textSamp03_UL, 3);	// Texture unit 3


	GLint tex0_ratio_UL = glGetUniformLocation(shaderProgID, "tex_0_3_ratio");
	glUniform4f(tex0_ratio_UL,
				material->textureRatio[0],		// 1.0
				material->textureRatio[1],
				material->textureRatio[2],
				material->textureRatio[3]);


	return;
}

void RenderingEngine::Render(iObject* object)
{
	cMaterial* material = object->GetComponent<cMaterial>();
	if (material != nullptr)
	{

		int shaderProgID = cShaderManager::Instance()->getIDFromFriendlyName(material->shaderName);
		if (cShaderManager::Instance()->last_used_shader_id != shaderProgID)
		{
			cShaderManager::Instance()->useShaderProgram(shaderProgID);
			float time = glfwGetTime();
			glUniform1f(glGetUniformLocation(shaderProgID, "iTime"), time);

			// set resolution
			glUniform2f(glGetUniformLocation(shaderProgID, "iResolution"),
						width,
						height);
		}


		sTransform& transform = object->transform;
		const cMaterial& mat = *material;

		mat4 m(1.f);
		m *= glm::translate(glm::mat4(1.f), transform.pos);
		m *= mat4(transform.rotation);

		mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));

		m*= glm::scale(glm::mat4(1.0f), transform.scale);


		GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgID, "isSkybox");
		if (!mat.isSkybox)
		{
			glUniform1f(bIsSkyBox_UL, (float)GL_FALSE);
			// Don't draw back facing triangles (default)
			glCullFace(GL_BACK);
			SetUpTextureBindingsForObject(material, shaderProgID);
		}
		else
		{
			// Draw the back facing triangles. 
			// Because we are inside the object, so it will force a draw on the "back" of the sphere 
			glCullFace(GL_FRONT);
			//glCullFace(GL_FRONT_AND_BACK);

			glUniform1f(bIsSkyBox_UL, (float)GL_TRUE);

			cBasicTextureManager* textureManager = cBasicTextureManager::Instance();

			GLuint skyBoxTextureID = textureManager->getTextureIDFromName("space");
			glActiveTexture(GL_TEXTURE26);				// Texture Unit 26
			glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);	// Texture now assoc with texture unit 0

			// Tie the texture units to the samplers in the shader
			GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgID, "skyBox");
			glUniform1i(skyBoxSampler_UL, 26);	// Texture unit 26
		}

		GLint matModelIT_UL = glGetUniformLocation(shaderProgID, "matModelInverTrans");
		glUniformMatrix4fv(matModelIT_UL, 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));

		GLint matModel_UL = glGetUniformLocation(shaderProgID, "matModel");
		GLint matView_UL = glGetUniformLocation(shaderProgID, "matView");
		GLint matProj_UL = glGetUniformLocation(shaderProgID, "matProj");

		glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(m));
		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(matProj_UL, 1, GL_FALSE, glm::value_ptr(projection));



		GLint eye_loc = glGetUniformLocation(shaderProgID, "eyeLocation");
		glUniform4f(eye_loc,
					camera.Eye.x,
					camera.Eye.y,
					camera.Eye.z,
					1.0f
		);

		GLint diff_loc = glGetUniformLocation(shaderProgID, "diffuseColour");
		glUniform4f(diff_loc,
					mat.colour.x,
					mat.colour.y,
					mat.colour.z,
					mat.colour.w
		);

		GLint spec_loc = glGetUniformLocation(shaderProgID, "specularColour");
		glUniform4f(spec_loc,
					mat.specColour.x,
					mat.specColour.y,
					mat.specColour.z,
					mat.specIntensity
		);

		GLint isUniform_location = glGetUniformLocation(shaderProgID, "isUniform");
		glUniform1i(isUniform_location, mat.uniformColour);


		cLightManager::Instance()->Set_Light_Data(shaderProgID);

		if (mat.isWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		sModelDrawInfo drawInfo;
		if (cVAOManager::Instance()->FindDrawInfoByModelName(mat.meshName, drawInfo))
		{
			glBindVertexArray(drawInfo.VAO_ID);
			glDrawElements(GL_TRIANGLES,
						   drawInfo.numberOfIndices,
						   GL_UNSIGNED_INT,
						   0);
			glBindVertexArray(0);
		}
	}
}
