#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct sTransform
{
private:
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;
	typedef glm::quat quat;
	typedef glm::mat4 mat4;

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

	inline vec3 Position()	{ return pos; }
	inline quat Rotation()	{ return rotation; }
	inline vec3 Scale()		{ return scale; }

	inline void Position(const vec3& position)	{ pos = position; }
	inline void Rotation(const quat& rotation)	{ this->rotation = rotation; }
	inline void Scale(const vec3& scale)		{ this->scale = scale; }

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

	inline mat4 ModelMatrix()
	{
		return ((glm::mat4(1.f) * TranslationMatrix()) * RotationMatrix()) * ScaleMatrix();
	}

	inline mat4 TranslationMatrix()
	{
		return glm::translate(mat4(1.0f), pos);
	}

	inline mat4 TranslationMatrix(sTransform& parent)
	{
		return glm::translate(mat4(1.0f),
							  vec3(parent.ModelMatrix() * vec4(pos, 1.0f))
		);
	}

	inline mat4 RotationMatrix()
	{
		return mat4(1.f) * mat4(rotation);
	}

	inline mat4 RotationMatrix(sTransform& parent)
	{
		return mat4(1.f) * mat4(rotation * parent.rotation);
	}

	inline mat4 ScaleMatrix()
	{
		return glm::scale(mat4(1.0f), scale);
	}

	inline mat4 ScaleMatrix(sTransform& parent)
	{
		return glm::scale(mat4(1.0f),
			vec3(
				scale.x * parent.scale.x,
				scale.y * parent.scale.y,
				scale.z * parent.scale.z
			)
		);
	}

};