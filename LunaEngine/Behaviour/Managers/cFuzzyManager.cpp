#include "cFuzzyManager.h"
#include <cGameObject.h>
#include <Shader/Shader.h>
#include <EntityManager/cEntityManager.h>
#include <Physics/global_physics.h>
#include <Physics/Mathf.h>
#include <Behaviour/AI/cFuzzyLogicBehaviour.h>

namespace AI {

	void cFuzzyManager::start()
	{
		sTextureData texture;
		if (!cBasicTextureManager::Instance()->LoadBMPFromFile("assets/resourceMap3.bmp", texture))
		{
			printf("Couldn't load bmp\n");
			return;
		}

		graph = new Graph(texture);

		for (auto& node : graph->nodes)
		{
			cGameObject* obj = new cGameObject;
			obj->shader = Shader::FromName("basic");
			obj->meshName = "cube";
			obj->texture[0].SetTexture("blue.png", 1.f);
			obj->transform.Position(node->position);
			if (node->tag == '_')
			{
				obj->transform.scale.y = 2.f;
				obj->texture[0].SetTexture("WATER_BUMP.png", 1.f);

				nPhysics::sCubeDef def;
				def.mass = 0.f;
				def.Offset = vec3(0.f);
				def.Scale = obj->transform.Scale();
				def.velocity = vec3(0.f);

				obj->AddComponent(g_PhysicsFactory->CreateCube(obj, def));

				cEntityManager::Instance().AddEntity(obj);
			}
			else if (node->tag == 'g')
			{
				obj->meshName = "ship";
				obj->texture[0].SetTexture("Fighter_Ship_body_diffuse.png");
				obj->transform.pos.y += 0.6f;
				obj->transform.Scale(vec3(0.0007f));
				obj->transform.UpdateEulerRotation(vec3(0.f, Mathf::randInRange(0.f, 359.f), 0.f));
				nPhysics::sSphereDef def;
				def.mass = 1.f;
				def.Offset = vec3(0.f);
				def.Radius = 0.6f;
				def.velocity = vec3(0.f);

				auto* component = g_PhysicsFactory->CreateSphere(obj, def);
				component->setIsRotateable(false);
				obj->AddComponent(component);
				obj->AddComponent<cFuzzyLogicBehaviour>();

				cEntityManager::Instance().AddEntity(obj);
			}
			else
			{
				delete obj;
			}
		}
	}

	void cFuzzyManager::update(float dt)
	{

	}

}