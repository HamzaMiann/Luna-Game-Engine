#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include "iShape.h"

namespace phys
{
	struct sRigidBodyDef
	{
		float Mass;
		glm::vec3 Position;
		glm::vec3 Velocity;
	};

	class cRigidBody
	{
		friend class cWorld;
	public:
		//cRigidBody(const sRigidBodyDef& def, iShape* shape);
		cRigidBody(const sRigidBodyDef& def);

		void GetTransform(glm::mat4& transform);
		virtual void AddForce(const glm::vec3& force);
		void AddForceAtPoint(const glm::vec3& force, const glm::vec3& relativePoint);

		inline bool IsStatic() { return mMass == 0.f; }
		//inline const iShape* GetShape() { return mShape; }
		//inline const eShapeType& GetShapeType() { return mShape->GetShapeType(); }
		virtual inline const eShapeType& GetShape() = 0;

	protected:
		//iShape* mShape;
		// TODO: add/change as required:
		glm::vec3 mPosition;
		glm::vec3 mVelocity;
		glm::vec3 mAcceleration;
		float mMass;
		float mInvMass;

	private:
		

		// private, so as to not be used. like, ever
		// TODO: private constructors
		cRigidBody() = delete;
		cRigidBody(const cRigidBody& other) = delete;
	};

	
}