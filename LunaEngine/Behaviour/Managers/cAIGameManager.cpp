#include "cAIGameManager.h"
#include <Physics/global_physics.h>
#include <EntityManager/cEntityManager.h>
#include <cGameObject.h>
#include <Behaviour/Controls/cPlayerBehaviour.h>
#include <Physics/Mathf.h>
#include <Components/ComponentFactory.h>

namespace _GameManager_
{
	cGameObject* MakeShip(glm::vec3 const& position)
	{
		cGameObject* obj = new cGameObject;
		obj->transform.pos = position;
		obj->transform.scale = glm::vec3(0.000731f);
		obj->meshName = "ship";
		obj->tag = "enemy";
		obj->shaderName = "basic";
		obj->specColour = glm::vec3(0.9f);
		obj->specIntensity = 10.f;
		obj->texture[0] = "blue.bmp";
		obj->textureRatio[0] = 1.f;
		nPhysics::sSphereDef def;
		def.gravity_factor = 1.f;
		def.mass = 1.0f;
		def.Offset = glm::vec3(0.f);
		def.Radius = 0.9f;
		def.velocity = glm::vec3(0.f);
		nPhysics::iSphereComponent* body = g_PhysicsFactory->CreateSphere(obj, def);
		obj->AddComponent(body);
		return obj;
	}
}

void cAIGameManager::start()
{
}

void cAIGameManager::update(float dt)
{
	time_passed += dt;
	if (time_passed > 7.f)
	{
		vec3 pos = vec3(
			Mathf::randInRange(-15.f, 15.f),
			1.f,
			Mathf::randInRange(-18.f, 18.f)
		);
		cGameObject* newObject = _GameManager_::MakeShip(pos);
		int index = Mathf::randInRange(0, 3);
		switch (index)
		{
		case 0: ComponentFactory::GetComponent("AISeekBehaviour", newObject); newObject->texture[0] = "yellow.bmp"; break;
		case 1: ComponentFactory::GetComponent("AIWanderBehaviour", newObject); break;
		case 2: ComponentFactory::GetComponent("AIPursueBehaviour", newObject); newObject->texture[0] = "red.bmp"; break;
		case 3: ComponentFactory::GetComponent("AIApproachBehaviour", newObject); newObject->texture[0] = "Grass.bmp"; break;
		}
		
		cEntityManager::Instance()->AddEntity(newObject);

		time_passed = 0.f;
	}
}


void cAIGameManager::Player_Shoot(glm::vec3 const& start, glm::vec3 const& velocity)
{
	cGameObject* obj = new cGameObject;
	obj->transform.pos = start;
	obj->transform.scale = vec3(.3f);
	obj->meshName = "sphere";
	obj->texture[0] = "blue.bmp";
	obj->shaderName = "basic";
	obj->textureRatio[0] = 0.f;
	nPhysics::sSphereDef def;
	def.gravity_factor = 1.f;
	def.mass = 0.5f;
	def.Offset = vec3(0.f);
	def.Radius = 0.3f;
	def.velocity = velocity;
	nPhysics::iSphereComponent* body = g_PhysicsFactory->CreateSphere(obj, def);
	obj->AddComponent(body);
	cBullet* bullet = obj->AddComponent<cBullet>();
	bullet->attack_layer = "enemy";
	bullet->manager = this;
	cEntityManager::Instance()->AddEntity(obj);
	player_bullets.push_back(&obj->transform);
}

void cAIGameManager::Enemy_Shoot(glm::vec3 const& start, glm::vec3 const& velocity)
{
	cGameObject* obj = new cGameObject;
	obj->transform.pos = start;
	obj->transform.scale = vec3(.3f);
	obj->meshName = "sphere";
	obj->shaderName = "basic";
	obj->texture[0] = "red.bmp";
	obj->textureRatio[0] = 1.f;
	nPhysics::sSphereDef def;
	def.gravity_factor = 0.f;
	def.mass = 0.5f;
	def.Offset = vec3(0.f);
	def.Radius = 0.3f;
	def.velocity = velocity;
	nPhysics::iSphereComponent* body = g_PhysicsFactory->CreateSphere(obj, def);
	obj->AddComponent(body);
	cBullet* bullet = obj->AddComponent<cBullet>();
	bullet->attack_layer = "player";
	bullet->manager = this;
	cEntityManager::Instance()->AddEntity(obj);
}

void cAIGameManager::cBullet::start()
{
}

void cAIGameManager::cBullet::update(float dt)
{
	t += dt;
	if (t > life_time)
	{
		cEntityManager::Instance()->RemoveEntity(dynamic_cast<cGameObject*>(&this->parent));
	}
}

void cAIGameManager::cBullet::OnCollide(iObject* other)
{
	cGameObject* obj = dynamic_cast<cGameObject*>(other);
	if (obj)
	{
		if (obj->tag == attack_layer)
		{
			if (attack_layer == "player")
			{
				cEntityManager::Instance()->RemoveEntity(dynamic_cast<cGameObject*>(&this->parent));
				cPlayerBehaviour* player = obj->GetComponent<cPlayerBehaviour>();
				player->Reset();
				auto it = std::find(manager->player_bullets.begin(),
					manager->player_bullets.end(),
					&this->transform);
				if (it != manager->player_bullets.end()) manager->player_bullets.erase(it);
			}
			else
			{
				cEntityManager::Instance()->RemoveEntity(dynamic_cast<cGameObject*>(&this->parent));
				cEntityManager::Instance()->RemoveEntity(obj);
				auto it = std::find(manager->player_bullets.begin(),
					manager->player_bullets.end(),
					&this->transform);
				if (it != manager->player_bullets.end()) manager->player_bullets.erase(it);
			}
		}
	}
}
