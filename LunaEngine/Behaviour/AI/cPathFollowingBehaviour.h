#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#endif

#include <Components/cAnimationController.h>

struct GraphNode;

namespace AI
{
	class cPathFindingManager;

	enum class STATE
	{
		NONE,
		WAITING_FOR_KEY,
		GOING_TO_RESOURCE,
		COLLECTING_RESOURCE,
		GOING_HOME,
		DELIVERING
	};

	class cPathFollowingBehaviour : public iAIBehaviour
	{
	public:
		virtual ~cPathFollowingBehaviour() {}
		cPathFollowingBehaviour(iObject* parent)
			: iAIBehaviour(parent)
		{
		}

		cAnimationController* anim;
		cPathFindingManager* manager;
		std::vector<GraphNode*> path;
		STATE state;
		GraphNode* current;
		float resourceTimer;
		float deliverTimer;
		iObject* resource = 0;

		// Inherited via iAIBehaviour
		virtual bool serialize(rapidxml::xml_node<>* root_node) override { return false; }

		virtual bool deserialize(rapidxml::xml_node<>* root_node) override { return false; }

		virtual void start() override;

		virtual void update(float dt) override;

		void FindResource();
		void FollowPath(float dt);
		void CollectResource(float dt);
		void Deliver(float dt);

	};
};