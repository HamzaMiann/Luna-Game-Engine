#pragma once

#include "iAIBehaviour.h"

namespace AI
{
	class cSeekBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cSeekBehaviour() {}
		cSeekBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{ }


        nPhysics::iPhysicsComponent* rb;
        sTransform* target;
        float mDt;
		float slowingRadius;
		float maxVelocity;

		void SeekArrive();

		// Inherited via iAIBehaviour
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;

		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		virtual void start() override;

		virtual void update(float dt) override;

	};
};