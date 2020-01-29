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

		cPlaneBody(const sRigidBodyDef& def)
			: iShape(eShapeType::plane),
			mNormal(glm::vec3(0.f, 1.f, 0.1)),
			mConstant(0.f),
			cRigidBody(def)
		{
		}

		inline const glm::vec3& GetNormal() { return mNormal; }
		inline float GetConstant() { return mConstant; }

		virtual inline const eShapeType& GetShape() override { return GetShapeType(); }

	protected:
		glm::vec3 mNormal;
		float mConstant;

	private:
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

		cSphereBody(const sRigidBodyDef& def)
			: iShape(eShapeType::sphere),
			mPos(glm::vec3(0.f)),
			mRadius(1.f),
			cRigidBody(def)
		{
		}

		virtual inline const eShapeType& GetShape() override { return GetShapeType(); }

	protected:
		glm::vec3 mPos;
		float mRadius;

	private:

		cSphereBody() = delete;
		cSphereBody(const cSphereBody& other) = delete;
		cSphereBody& operator=(const cSphereBody& other) = delete;
	};
}