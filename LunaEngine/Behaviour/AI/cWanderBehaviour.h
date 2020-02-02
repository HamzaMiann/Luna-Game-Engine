#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#endif

namespace AI
{
	class cWanderBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cWanderBehaviour() {}
		cWanderBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{
		}


		nPhysics::iPhysicsComponent* rb;
		float mDt;
		float maxVelocity;
		float distanceToCircle;
		float circleRadius;

		float wander_time = 0.f;
		float idle_time = 0.f;

		void Wander();

		// Inherited via iAIBehaviour
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;

		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		virtual void start() override;

		virtual void update(float dt) override;

	};
};