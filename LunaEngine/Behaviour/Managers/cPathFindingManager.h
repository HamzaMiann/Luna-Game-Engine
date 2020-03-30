#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include <Behaviour/AI/cFormationBehaviour.h>
#include "Graph.h"
class cGameObject;

namespace AI
{
	class cPathFindingManager : public iBehaviour
	{
	public:

		virtual ~cPathFindingManager() {}

		cPathFindingManager(iObject* root) :
			iBehaviour(root)
		{
		}

		virtual void start() override;
		virtual void update(float dt) override;

	private:
		bool shouldShowGraph = false;

		Graph* graph;

		GraphNode* playerNode;
		GraphNode* home;
		std::vector<GraphNode*> resources;
	};
}