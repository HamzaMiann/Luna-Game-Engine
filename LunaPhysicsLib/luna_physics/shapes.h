#pragma once

#include "iShape.h" 
#include "cRigidBody.h"
#include <glm/vec3.hpp>

namespace phys
{
	class cPlaneBody : public iShape, public cRigidBody
	{
	public:
		virtual ~cPlaneBody() {}

		cPlaneBody(const sRigidBodyDef& def, const glm::vec3& normal, float constant)
			: iShape(eShapeType::plane),
			mNormal(normal),
			mConstant(constant),
			cRigidBody(def)
		{
		}

		inline const glm::vec3& GetNormal() { return mNormal; }
		inline float GetConstant() { return mConstant; }

		virtual inline const eShapeType& GetShape() override { return GetShapeType(); }

	private:
		glm::vec3 mNormal;
		float mConstant;

		// private, so as to not be used. like, ever.

		cPlaneBody() = delete;
		cPlaneBody(const cPlaneBody& other) = delete;
		cPlaneBody& operator=(const cPlaneBody& other) = delete;
	};

	class cSphereBody : public iShape, public cRigidBody
	{
	public:
		virtual ~cSphereBody() {}

		cSphereBody(const sRigidBodyDef& def, const glm::vec3& position, float radius)
			: iShape(eShapeType::sphere),
			mPos(position),
			mRadius(radius),
			cRigidBody(def)
		{
		}

		virtual inline const eShapeType& GetShape() override { return GetShapeType(); }

	private:
		glm::vec3 mPos;
		float mRadius;
	};
}