#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#endif

class cAIGameManager;

namespace AI
{
	class cPursueBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cPursueBehaviour() {}
		cPursueBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{
		}

		cAIGameManager* manager;
		nPhysics::iPhysicsComponent* rb;
		nPhysics::iPhysicsComponent* target_rb;
		sTransform* target;
		float mDt;
		float maxVelocity;

		void Pursue();
		void Evade(glm::vec3 position, glm::vec3 velocity);

		// Inherited via iAIBehaviour
		virtual bool serialize(rapidxml::xml_node<>* root_node) override;

		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		virtual void start() override;

		virtual void update(float dt) override;

	};
};