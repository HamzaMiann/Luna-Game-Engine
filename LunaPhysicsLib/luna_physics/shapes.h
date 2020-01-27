#pragma once

#include "iShape.h"
#include <glm/vec3.hpp>

namespace phys
{
	class cPlane : public iShape
	{
	public:
		virtual ~cPlane() {}

		cPlane(const glm::vec3& normal, float constant)
		: iShape(eShapeType::plane),
			mNormal(normal),
			mConstant(constant)
		{
		}

		inline const glm::vec3& GetNormal() { return mNormal; }
		inline float GetConstant() { return mConstant; }

	private:
		glm::vec3 mNormal;
		float mConstant;

		// private, so as to not be used. like, ever.

		cPlane() = delete;
		cPlane(const cPlane& other) = delete;
		cPlane& operator=(const cPlane& other) = delete;
	};

	class cSphere : public iShape
	{
	public:
		virtual ~cSphere() {}

		cSphere(const glm::vec3& position, float radius)
			: iShape(eShapeType::sphere),
			mPos(position),
			mRadius(radius)
		{
		}

	private:
		glm::vec3 mPos;
		float mRadius;
	};
}