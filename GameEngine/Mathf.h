#pragma once

#include <stdlib.h>			// For rand()
#include <math.h>
#include <glm/vec3.hpp>
#define PI 3.14159265

namespace Mathf
{

	static void rotate_vector(float angle, glm::vec3 const& center, glm::vec3& point)
	{
		angle = angle * (PI / 180.f);
		float s = sin(angle);
		float c = cos(angle);

		float xnew = point.x * c - point.z * s;
		float znew = point.z * c + point.x * s;
		point.x = xnew + center.x;
		point.z = znew + center.z;
	}

	static glm::vec3 get_rotated_vector(float angle, glm::vec3 const& center, glm::vec3 const& point)
	{
		angle = angle * (PI / 180.f);
		float s = sin(angle);
		float c = cos(angle);

		float xnew = point.x * c - point.z * s;
		float znew = point.z * c + point.x * s;

		return glm::vec3(
			xnew + center.x,
			point.y,
			znew + center.z
		);
	}

	static glm::vec3 get_direction_vector(glm::vec3 const& from, glm::vec3 const& to)
	{
		return (to - from);
	}

	static glm::vec3 get_reverse_direction_vector(glm::vec3 const& from, glm::vec3 const& to)
	{
		return (from - to);
	}


	template <class T>
	T randInRange(T min, T max)
	{
		double value =
			min + static_cast <double> (rand())
			/ (static_cast <double> (RAND_MAX / (static_cast<double>(max - min))));
		return static_cast<T>(value);
	};

}