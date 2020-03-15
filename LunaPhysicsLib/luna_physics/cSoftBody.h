#pragma once
#include <glm/glm_common.h>
#include <vector>
#include "shapes.h"
#include "cRigidBody.h"

namespace phys
{
	struct sSoftBodyNodeDef
	{
		vec3 Position;
		float Mass;
		float Radius;
	};
	struct sSoftBodyDef
	{
		std::vector<sSoftBodyNodeDef> Nodes;
		std::vector<std::pair<size_t, size_t>> Springs;
		float SpringConstant;
		float PercentOfGravityApplied;
		vec3 windForce;
	};

	class cSoftBody : public iBody
	{
	private:
		class cSpring;
		class cNode
		{
		public:
			cNode(const sSoftBodyNodeDef& nodeDef);
			void CalculateRadius();
			bool IsNeighbour(cNode* other);
			inline bool isFixed()
			{
				return Mass == 0.f;
			}
			std::vector<cSpring*> Springs;
			float Mass;
			float radius;
			vec3 Previous;
			vec3 Position;
			vec3 Velocity;
			vec3 Acceleration;
		};
		class cSpring
		{
		public:
			cSpring(cNode* nodeA, cNode* nodeB, float springConstant);
			void UpdateSpringForce();
			void ApplyForceToNodes();
			cNode* GetOther(cNode* node);
			bool IsNeighbour(cNode* other);
			cNode* NodeA;
			cNode* NodeB;
			float SpringConstant;
			float RestingLength;
			vec3 SpringForceAtoB;
		};
	public:
		cSoftBody(const sSoftBodyDef& def);
		virtual ~cSoftBody();

		/*
		SET ACCELERATIONS FOR ALL NODES TO ZERO
		*/
		void ClearAccelerations();

		/*
		INTEGRATE ALL NODES USING EULER INTEGRATION
		*/
		void Integrate(float dt, const vec3& gravity);

		/*
		COLLIDE WITH A GIVEN RIGID-BODY
		*/
		void CollideWith(cRigidBody* body);

		/*
		GET SIZE OF NODE VECTOR (num nodes)
		*/
		size_t size();

		bool NodeRadius(size_t index, float& radiusOut);
		bool NodePosition(size_t index, vec3& positionOut);

	protected:

		/*
		INTEGRATE A SPECIFIC NODE USING EULER INTEGRATION
		*/
		void IntegrateNode(cNode* node, float dt);

		/*
		COLLIDE SPHERE BODY WITH ALL NODES
		*/
		bool CollideSphere(cRigidBody* body, cSphere* shape);

		/*
		COLLIDE PLANE BODY WITH ALL NODES
		*/
		bool CollidePlane(cRigidBody* body, cPlane* shape);

		/*
		COLLIDE PLANE BODY WITH A SPECIFIC NODE
		*/
		bool CollidePlane(cNode* node, cPlane* shape);

		float PercentOfGravityApplied;
		float T;
		float mDt;
		vec3 windForce;
		std::vector<cNode*> mNodes;
		std::vector<cSpring*> mSprings;
	};
}