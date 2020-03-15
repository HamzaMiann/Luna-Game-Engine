#pragma once

#include <glm/glm_common.h>
#include "iBody.h"
#include "iShape.h"

namespace phys
{
	struct sRigidBodyDef
	{
		float Mass;
		float GravityFactor;
		vec3 Position;
		vec3 Velocity;
	};

	class cRigidBody : public iBody
	{
		friend class cWorld;
		friend class cSoftBody;
	public:
		cRigidBody(const sRigidBodyDef& def, iShape* shape);

		void GetTransformFromBody(mat4& transform);
		void AddForceToCenter(const vec3& force);
		void AddForceAtPoint(const vec3& force, const vec3& relativePoint);

		inline bool IsStatic() { return mMass == 0.f; }
		inline iShape* GetShape() { return mShape; }
		inline const eShapeType& GetShapeType() { return mShape->GetShapeType(); }

		inline void SetShape(iShape* shape) { mShape = shape; }

		virtual void Collided(cRigidBody* other) {}

	protected:
		iShape* mShape;
		// TODO: add/change as required:
		vec3 mPosition;
		vec3 mPreviousPosition;
		vec3 mVelocity;
		vec3 mAcceleration;
		float mMass;
		float mInvMass;
		float mGravityFactor;

	private:
		// private, so as to not be used. like, ever
		// TODO: private constructors
		cRigidBody() = delete;
		cRigidBody(const cRigidBody& other) = delete;
	};

	
}