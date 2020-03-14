#include "cRenderMesh.h"
#include <Camera.h>
#include <Components\cAnimationController.h>
#include <Texture\cBasicTextureManager.h>

cRenderMesh::cRenderMesh(iObject* obj):
	iRenderMesh(*obj),
	specColour(glm::vec4(.5f, .5f, .5f, 1.0f)),
	isUniformColour(false),
	isWireframe(false),
	isSkybox(false),
	reflectivity(0.f),
	refractivity(0.f),
	shader(0)
{
}

cRenderMesh::~cRenderMesh()
{
}

bool cRenderMesh::deserialize(rapidxml::xml_node<>* root_node)
{
	return false;
}

void cRenderMesh::_render()
{
	if (shader && mesh_data.HasMesh())
	{
		_render_mesh(*shader, *mesh_data.drawInfo);
	}
}

void cRenderMesh::_set_textures(Shader& shader)
{
	//if (pass_id != 1) return;

	shader.SetTexture(texture[0].GetID(), "textSamp00", 0);
	shader.SetTexture(texture[1].GetID(), "textSamp01", 1);
	shader.SetTexture(texture[2].GetID(), "textSamp02", 2);
	shader.SetTexture(texture[3].GetID(), "textSamp03", 3);

	shader.SetVec4("tex_tiling",
		vec4(
			texture[0].GetTiling(),
			texture[1].GetTiling(),
			texture[2].GetTiling(),
			texture[3].GetTiling()
		)
	);

	shader.SetVec4("tex_0_3_ratio",
		vec4(
			texture[0].GetBlend(),
			texture[1].GetBlend(),
			texture[2].GetBlend(),
			texture[3].GetBlend()
		)
	);

}

void cRenderMesh::_render_mesh(Shader& shader, sModelDrawInfo& mesh)
{
	Camera& camera = *Camera::main_camera;

	/*

	MISC EFFECTS

	*/
	bool isUnique = parent.tag == "ground";
	shader.SetBool("isUnique", (float)isUnique);


	/*

	ANIMATION

	*/
	cAnimationController* animator = parent.GetComponent<cAnimationController>();

	if (animator)
	{
		auto& transforms = animator->GetTransformations();
		shader.SetBool("isSkinnedMesh", (float)GL_TRUE);
		shader.SetMat4Array("matBonesArray", transforms);
	}
	else shader.SetBool("isSkinnedMesh", (float)GL_FALSE);


	/*

	REFLECTION AND REFRACTION

	*/
	shader.SetBool("reflectivity", reflectivity);
	shader.SetBool("refractivity", refractivity);


	/*

	SKYBOX

	*/
	shader.SetCubemap("skyBox", cBasicTextureManager::Instance()->getTextureIDFromName("default"));
	if (isSkybox)
	{
		// Don't draw back facing triangles (default)
		glCullFace(GL_BACK);
		shader.SetBool("isSkybox", GL_FALSE);
		_set_textures(shader);
	}
	else
	{
		// Draw the back facing triangles. 
		// Because we are inside the object, so it will force a draw on the "back" of the sphere 
		glCullFace(GL_FRONT);
		shader.SetBool("isSkybox", GL_TRUE);
	}

	mat4 m(1.f);

	if (parent.parent)
	{
		m *= transform.TranslationMatrix(parent.parent->transform);
		m *= transform.RotationMatrix(parent.parent->transform);
	}
	else
	{
		m *= transform.TranslationMatrix();
		m *= transform.RotationMatrix();
	}



	mat4 matModelInverseTranspose = glm::inverse(glm::transpose(m));
	glUniformMatrix4fv(shader["matModelInverTrans"], 1, GL_FALSE, glm::value_ptr(matModelInverseTranspose));


	if (parent.parent)
		m *= transform.ScaleMatrix(parent.parent->transform);
	else
		m *= transform.ScaleMatrix();

	//if (pass_id != 1) m = mat4(1.f);

	shader.SetMat4("matModel", m);

	shader.SetVec4("eyeLocation",
		vec4(
			camera.Eye.x,
			camera.Eye.y,
			camera.Eye.z,
			1.0f
		)
	);

	shader.SetVec4("eyeTarget",
		vec4(
			camera.Target.x,
			camera.Target.y,
			camera.Target.z,
			1.0f
		)
	);

	/*shader.SetVec4("diffuseColour",
		vec4(
			object.colour.x,
			object.colour.y,
			object.colour.z,
			object.colour.w
		)
	);*/

	shader.SetVec4("specularColour",
		vec4(
			specColour.x,
			specColour.y,
			specColour.z,
			specColour.w
		)
	);

	//shader.SetBool("isUniform", object.uniformColour);


	/*if (pass_id != 1)
	{
		cLightManager::Instance()->Set_Light_Data(shader);
		shader.SetVec2("lightPositionOnScreen", screenPos);
	}*/


	// This will change the fill mode to something 
	//  GL_FILL is solid 
	//  GL_LINE is "wireframe"
	//glPointSize(15.0f);

	if (isWireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glBindVertexArray(mesh.VAO_ID);
	glDrawElements(
		GL_TRIANGLES,
		mesh.numberOfIndices,
		GL_UNSIGNED_INT,
		0
	);
	glBindVertexArray(0);
	

	/*sModelDrawInfo drawInfo;
	if (cVAOManager::Instance().FindDrawInfoByModelName(object.meshName, drawInfo))
	{
		glBindVertexArray(drawInfo.VAO_ID);
		glDrawElements(
			GL_TRIANGLES,
			drawInfo.numberOfIndices,
			GL_UNSIGNED_INT,
			0
		);
		glBindVertexArray(0);
	}*/
}

void cRenderMesh::cMeshData::SetMesh(std::string meshName)
{
	sModelDrawInfo info;
	if (cVAOManager::Instance().FindDrawInfoByModelName(meshName, info))
	{
		mesh = cVAOManager::Instance().FindMeshByModelName(meshName);
		drawInfo = &info;
	}
	else
	{
		drawInfo = 0;
		mesh = 0;
	}
}
