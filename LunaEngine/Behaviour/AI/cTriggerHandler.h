#pragma once

#ifndef _iAIBehaviour_HG_
#define _iAIBehaviour_HG_
#include <interfaces/Behaviour/iAIBehaviour.h>
#include <Behaviour/Managers/cZombieGameManager.h>
#endif

class cGameObject;

namespace AI
{
	class cTriggerHandler : public iAIBehaviour
	{
	public:
		virtual ~cTriggerHandler() {}
		cTriggerHandler(iObject* parent)
			: iAIBehaviour(parent)
		{ }

		// Inherited via iAIBehaviour
		virtual bool deserialize(rapidxml::xml_node<>* root_node) override;

		virtual void start() override;

		virtual void update(float dt) override;

		virtual void OnCollide(iObject* other) override;

		std::string tombName;
		int lightIndex;

		cGameObject* tomb;
		cZombieGameManager* manager;

		bool canActivate = false;
	};
};