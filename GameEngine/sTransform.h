#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

struct sTransform
{
private:
	typedef glm::vec3 vec3;
	typedef glm::quat quat;

public:
	quat rotation;
	vec3 pos;
	vec3 scale;

	sTransform()
	{
		rotation = quat(vec3(0.f));
		pos = vec3(0.f);
		scale = vec3(1.f);
	}

	void SetEulerRotation(vec3 EulerAngleDegrees)
	{
		vec3 EulerAngleRadians(
			glm::radians(EulerAngleDegrees.x),
			glm::radians(EulerAngleDegrees.y),
			glm::radians(EulerAngleDegrees.z)
		);

		rotation = quat(EulerAngleRadians);
	}
	void UpdateEulerRotation(vec3 EulerAngleDegrees)
	{
		vec3 EulerAngleRadians(
			glm::radians(EulerAngleDegrees.x),
			glm::radians(EulerAngleDegrees.y),
			glm::radians(EulerAngleDegrees.z)
		);

		this->rotation *= quat(EulerAngleRadians);
	}

	vec3 EulerAngles(void) { return glm::eulerAngles(rotation); }
};