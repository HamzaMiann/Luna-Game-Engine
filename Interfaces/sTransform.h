#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

	inline glm::mat4 ModelMatrix()
	{
		return ((glm::mat4(1.f) * TranslationMatrix()) * RotationMatrix()) * ScaleMatrix();
	}

	inline glm::mat4 TranslationMatrix()
	{
		return glm::translate(glm::mat4(1.0f),
							  glm::vec3(pos.x,
										pos.y,
										pos.z)
		);
	}

	inline glm::mat4 RotationMatrix()
	{
		return glm::mat4(1.f) * glm::mat4(rotation);
	}

	inline glm::mat4 ScaleMatrix()
	{
		return glm::scale(glm::mat4(1.0f),
						  glm::vec3(scale.x,
									scale.y,
									scale.z)
		);
	}

};