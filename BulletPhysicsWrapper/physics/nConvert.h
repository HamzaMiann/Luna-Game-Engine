#pragma once

#include <glm/glm_common.h>
#include "btBulletDynamicsCommon.h"

namespace nConvert
{
	inline btVector3 ToBullet(const glm::vec3& vec)
	{
		return btVector3(vec.x, vec.y, vec.z);
	}

	inline btQuaternion ToBullet(const glm::quat& rot)
	{
		return btQuaternion(rot.x, rot.y, rot.z, rot.w);
	}

	inline glm::vec3 ToGLM(const btVector3& vec)
	{
		return glm::vec3(vec.x(), vec.y(), vec.z());
	}

	inline glm::quat ToGLM(const btQuaternion& quaternion)
	{
		return glm::quat(quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
	}

	inline glm::mat4 ToGLM(const btTransform& transform)
	{
		glm::mat4 mat(1.f);
		transform.getOpenGLMatrix(&mat[0][0]);
		return mat;
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut)
	{
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}
}