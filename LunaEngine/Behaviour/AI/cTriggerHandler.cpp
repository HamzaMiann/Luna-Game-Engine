#include "cTriggerHandler.h"
#include <EntityManager/cEntityManager.h>
#include <Rendering/RenderingEngine.h>
#include <InputManager.h>
#include <Audio/AudioEngine.hpp>
#include <Lighting/cLightManager.h>

namespace AI
{
	bool cTriggerHandler::deserialize(rapidxml::xml_node<>* root_node)
	{
		tombName = root_node->first_attribute("id")->value();
		lightIndex = std::atoi(root_node->first_attribute("lightID")->value());
		return true;
	}

	void cTriggerHandler::start()
	{
		tomb = cEntityManager::Instance().GetGameObjectByTag(tombName);
		manager = cEntityManager::Instance().GetObjectByTag("gameManager")->GetComponent<cZombieGameManager>();
		cLightManager::Instance()->Lights[lightIndex]->position = vec4(transform.Position(), 1.f);
	}

	void cTriggerHandler::update(float dt)
	{
		if (canActivate)
		{
			RenderingEngine::Instance().UITexture.SetTexture("eToActivate.png");
			if (Input::KeyUp(GLFW_KEY_F))
			{
				cEntityManager::Instance().RemoveEntity(&this->parent);
				tomb->texture[0].SetTexture("fx3_Panels_Color.png");
				AudioEngine::Instance()->PlaySound("checkpoint");
				cLightManager::Instance()->Lights[lightIndex]->param2.x = 1.0f;
				manager->ActivateTomb();
			}
		}

		canActivate = false;
	}

	void cTriggerHandler::OnCollide(iObject* other)
	{
		if (other->tag == "player") canActivate = true;
	}

}