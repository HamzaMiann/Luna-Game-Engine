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
		inline const float GetConstant() { return mConstant; }

	protected:
		glm::vec3 mNormal;
		float mConstant;

	private:
		// private, so as to not be used. like, ever.

		cPlane() = delete;
		cPlane(const cPlane& other) = delete;
		cPlane& operator=(const cPlane& other) = delete;
	};

	class cSphere : public iShape
	{
	public:
		virtual ~cSphere() {}

		cSphere(const glm::vec3& offset, float radius)
			: iShape(eShapeType::sphere),
			mOffset(offset),
			mRadius(radius)
		{
		}

		inline const glm::vec3& GetOffset() { return mOffset; }
		inline const float GetRadius() { return mRadius; }

	protected:
		glm::vec3 mOffset;
		float mRadius;

	private:
		cSphere() = delete;
		cSphere(const cPlane& other) = delete;
		cSphere& operator=(const cSphere& other) = delete;
	};
}