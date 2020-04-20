#pragma once

#include <interfaces/Behaviour/iBehaviour.h>
#include "Graph.h"

namespace AI
{
	class cFuzzyManager : public iBehaviour
	{
	public:

		virtual ~cFuzzyManager() {}

		cFuzzyManager(iObject* root) :
			iBehaviour(root)
		{
		}

		virtual void start() override;
		virtual void update(float dt) override;

	private:

		bool shouldShowGraph = false;
		Graph* graph;
	};
}