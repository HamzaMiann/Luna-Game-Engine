#include "cSoftBody.h"
#include <numeric>
#include "nCollide.h"
#include <glm\gtx\projection.hpp>

//#define MAX_VELOCITY 5.f
//#define MAX_FORCE 3.f

namespace phys
{
	cSoftBody::cSpring::cSpring(cNode* nodeA, cNode* nodeB, float springConstant)
		: NodeA(nodeA)
		, NodeB(nodeB)
		, RestingLength(glm::distance(nodeA->Position, nodeB->Position)),
		SpringConstant(springConstant),
		SpringForceAtoB(.0f, 0.f, 0.f)
	{
	}
	void cSoftBody::cSpring::UpdateSpringForce()
	{
		//HOOKS LAW!@
		glm::vec3 sep = NodeB->Position - NodeA->Position;
		float dist = glm::length(sep);
		float x = dist - RestingLength;
		SpringForceAtoB = -glm::normalize(sep) * x * SpringConstant;
#ifdef MAX_FORCE
		if (SpringForceAtoB.length() > MAX_FORCE)
		{
			SpringForceAtoB = glm::normalize(SpringForceAtoB) * MAX_FORCE;
		}
#endif
	}

	void cSoftBody::cSpring::ApplyForceToNodes()
	{
		if (!NodeB->isFixed())
		{
			NodeB->Acceleration += SpringForceAtoB / NodeB->Mass;
		}
		if (!NodeA->isFixed())
		{
			NodeA->Acceleration -= SpringForceAtoB / NodeA->Mass;
		}
		//NodeA->Acceleration -= SpringForceAtoB / NodeA->Mass;
	}

	cSoftBody::cNode* cSoftBody::cSpring::GetOther(cNode* node)
	{
		return node == NodeA ? NodeB : NodeA;
	}

	cSoftBody::cSoftBody(const sSoftBodyDef& def)
		: iBody(eBodyType::soft),
		T(0.f),
		windForce(def.windForce)
	{
		size_t numNodes = def.Nodes.size();
		mNodes.resize(numNodes);
		for (size_t idx = 0; idx < numNodes; idx++)
		{
			mNodes[idx] = new cNode(def.Nodes[idx]);
		}

		size_t numSprings = def.Springs.size();
		mSprings.resize(numSprings);
		for (size_t idx = 0; idx < numSprings; idx++)
		{
			mSprings[idx] = new cSpring(mNodes[def.Springs[idx].first],
				mNodes[def.Springs[idx].second],
				def.SpringConstant);
			mNodes[def.Springs[idx].first]->Springs.push_back(mSprings[idx]);
			mNodes[def.Springs[idx].second]->Springs.push_back(mSprings[idx]);
		}

		PercentOfGravityApplied = def.PercentOfGravityApplied;
	}

	cSoftBody::~cSoftBody()
	{
		size_t numNodes = mNodes.size();
		for (size_t idx = 0; idx < numNodes; idx++)
		{
			delete mNodes[idx];
		}
		mNodes.clear();

		size_t numSprings = mSprings.size();
		for (size_t idx = 0; idx < numSprings; idx++)
		{
			delete mSprings[idx];
		}
		mSprings.clear();
	}
	void cSoftBody::ClearAccelerations()
	{
		for (size_t i = 0; i < size(); ++i)
		{
			mNodes[i]->Acceleration = glm::vec3(0.f);
		}
	}

	void cSoftBody::Integrate(float dt, const glm::vec3& gravity)
	{
		mDt = dt;
		ClearAccelerations();

		// Step 1: start with gravity
		for (size_t i = 0; i < size(); ++i)
		{
			mNodes[i]->Acceleration = gravity * PercentOfGravityApplied;
			mNodes[i]->Acceleration += windForce * (sin(T + sin(T)) + 0.5f);
		}

		// Step 2: Accumulate spring forces based on current positions
		for (size_t i = 0; i < mSprings.size(); ++i)
		{
			mSprings[i]->UpdateSpringForce();
			mSprings[i]->ApplyForceToNodes();
		}

		// Step 3: Integrate the nodes!
		for (size_t i = 0; i < size(); i++)
		{
			cNode* node = mNodes[i];
			IntegrateNode(node, dt);
		}
		T += dt;
	}

	void cSoftBody::CollideWith(cRigidBody* body)
	{
		if (body->GetShapeType() == eShapeType::sphere)
		{
			CollideSphere(body, reinterpret_cast<cSphere*>(body->GetShape()));
		}
		else if (body->GetShapeType() == eShapeType::plane)
		{
			CollidePlane(body, reinterpret_cast<cPlane*>(body->GetShape()));
		}
	}

	size_t cSoftBody::size()
	{
		return mNodes.size();
	}
	bool cSoftBody::NodeRadius(size_t index, float& radiusOut)
	{
		if (index >= mNodes.size()) return false;
		radiusOut = mNodes[index]->radius;
		return true;
	}
	bool cSoftBody::NodePosition(size_t index, glm::vec3& positionOut)
	{
		if (index >= mNodes.size()) return false;
		positionOut = mNodes[index]->Position;
		return true;
	}
	void cSoftBody::IntegrateNode(cNode* node, float dt)
	{
		if (node->isFixed()) return;

		node->Previous = node->Position;

		node->Velocity += node->Acceleration * dt;
		// dampen the velocity
		node->Velocity *= glm::pow(0.4f, dt);

#ifdef MAX_VELOCITY
		if (node->Velocity.length() > MAX_VELOCITY)
		{
			node->Velocity = glm::normalize(node->Velocity) * MAX_VELOCITY;
		}
#endif

		node->Position += node->Velocity * dt;
	}
	bool cSoftBody::CollideSphere(cRigidBody* body, cSphere* shape)
	{
		for (auto node : mNodes)
		{
			//if (glm::dot(glm::normalize(node->Position - body->mPosition), glm::normalize(body->mVelocity)) < 0.) continue;

			float d = glm::distance(body->mPosition, node->Position);
			d -= (node->radius + shape->GetRadius());
			if (d <= 0.1f)
			{
				vec3 U1x, U1y, U2x, U2y, V1x, V1y, V2x, V2y;

				float m1, m2, x1, x2;
				vec3 v1temp, v1, v2, v1x, v2x, v1y, v2y, x(node->Position - body->mPosition);

				glm::normalize(x);
				v1 = node->Velocity;
				x1 = dot(x, v1);
				v1x = x * x1;
				v1y = v1 - v1x;
				m1 = node->Mass;// 1.0f; //mass of 1

				x = x * -1.0f;
				v2 = body->mVelocity;
				x2 = dot(x, v2);
				v2x = x * x2;
				v2y = v2 - v2x;
				m2 = 1.f / body->mInvMass;//1.0f; //mass of 1

				node->Velocity = (glm::vec3(v1x * (m1 - m2) / (m1 + m2) + v2x * (2 * m2) / (m1 + m2) + v1y) / 4.0f) * 2.f;
				//node->Velocity /= glm::clamp(d, 0.2f, 1.f);
			}
		}
		return false;
	}

	bool cSoftBody::CollidePlane(cRigidBody* body, cPlane* shape)
	{
		for (auto node : mNodes)
		{
			CollidePlane(node, shape);
		}
		return false;
	}

	bool cSoftBody::CollidePlane(cNode* node, cPlane* shape)
	{
		float collisionValue = 0.3f;// IntersectMovingSpherePlane(...); // TODO

		glm::vec3 c = node->Previous;
		float r = node->radius;
		glm::vec3 v = node->Position - node->Previous;
		glm::vec3 n = shape->GetNormal();
		float d = shape->GetConstant();
		float t(0.f);
		glm::vec3 q;
		int result = nCollide::intersect_moving_sphere_plane(c, r, v, n, d, t, q);

		if (result == 0)
		{
			// no collision!
			return false;
		}
		if (result == -1)
		{
			// already colliding at the beginning of the timestep
			//sphereBody->mPosition = sphereBody->mPreviousPosition;
			// TODO: STUFF

			glm::vec3 pointOnPlane = nCollide::closest_point_on_plane(node->Previous, n, d);

			// figure out an impulse that should have the sphere escape the plane
			float distance = glm::distance(node->Previous, pointOnPlane);

			float targetDistance = r;
			glm::vec3 impulse = n * (targetDistance - distance) / mDt;

			// reset
			node->Position = node->Previous;

			// apply impulse
			node->Velocity += impulse;

			// re-integrate
			IntegrateNode(node, mDt);

			return true;
		}
		// collided!

		// REFLECT!
		node->Velocity = glm::reflect(node->Velocity, n);

		// Lose some energy
		glm::vec3 nComponent = glm::proj(node->Velocity, n);
		node->Velocity -= nComponent * 0.2f;

		// rewind
		node->Position = (c + v * t);

		// integrate
		IntegrateNode(node, mDt * (1.f - t));
		return true;
	}
	cSoftBody::cNode::cNode(const sSoftBodyNodeDef& nodeDef)
		: Position(nodeDef.Position),
		Mass(nodeDef.Mass),
		Velocity(0.f, 0.0f, 0.f),
		Acceleration(0.f, 0.f, 0.f),
		radius(nodeDef.Radius),
		Previous(nodeDef.Position)
	{
	}
	void cSoftBody::cNode::CalculateRadius()
	{
		float smallestDistance = std::numeric_limits<float>::max();
		size_t numNeighbors = this->Springs.size();
		for (size_t idx = 0; idx < numNeighbors; idx++)
		{
			//is this the smallest distance?
			float dist = glm::distance(Springs[idx]->GetOther(this)->Position, Position);
			if (dist < smallestDistance)
			{
				smallestDistance = dist;
			}
		}
		radius = smallestDistance * 0.5f; // TODO: tweak this value, maybe make it a constant somewhere?
	}
	bool cSoftBody::cNode::IsNeighbour(cNode* other)
	{
		for (size_t i = 0; i < Springs.size(); ++i)
		{
			if (Springs[i]->GetOther(this) == other)
			{
				return true;
			}
		}
	}
}