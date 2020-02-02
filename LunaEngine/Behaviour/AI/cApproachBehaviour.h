#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#endif

#include "../Controls/cPlayerBehaviour.h"

namespace AI
{
	class cApproachBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cApproachBehaviour() {}
		cApproachBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{
		}


		nPhysics::iPhysicsComponent* rb;
		cPlayerBehaviour* player_component;
		sTransform* target;
		float mDt;
		float slowingRadius;
		float maxVelocity;
		float reload_time;

		void Approach();

		// Inherited via iAIBehaviour
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;

		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		virtual void start() override;

		virtual void update(float dt) override;

	};
};