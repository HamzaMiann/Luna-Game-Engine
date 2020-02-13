#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include "btBulletDynamicsCommon.h"

namespace nConvert
{
	inline btVector3 ToBullet(const glm::vec3& vec)
	{
		return btVector3(vec.x, vec.y, vec.z);
	}

	inline glm::vec3 ToGLM(const btVector3& vec)
	{
		return glm::vec3(vec.x(), vec.y(), vec.z());
	}

	inline glm::quat ToGLM(const btQuaternion& quaternion)
	{
		return glm::quat(quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
	}

	inline void ToSimple(const btTransform& transformIn, glm::mat4& transformOut)
	{
		transformIn.getOpenGLMatrix(&transformOut[0][0]);
	}
}