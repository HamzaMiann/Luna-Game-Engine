#include "cEntityManager.h"
#include <cGameObject.h>
#include <queue>

namespace EntityManager
{
	std::queue<cGameObject*> remove_queue;
}

cEntityManager::cEntityManager()
{
	Entities = new std::vector<cGameObject*>();
}

cEntityManager::~cEntityManager()
{
	for (size_t i = 0; i < Entities->size(); ++i)
	{
		delete (*Entities)[i];
	}
	Entities->clear();
}

bool cEntityManager::AddEntity(cGameObject* entity)
{
	auto it = std::find(Entities->begin(), Entities->end(), entity);
	if (it == Entities->end())
	{
		Entities->push_back(entity);
		return true;
	}
	return false;
}

bool cEntityManager::RemoveEntity(cGameObject* entity)
{
	auto it = std::find(Entities->begin(), Entities->end(), entity);
	if (it != Entities->end())
	{
		EntityManager::remove_queue.push(entity);
		return true;
	}
	return false;
}

bool cEntityManager::RemoveEntity(iObject* entity)
{
	return RemoveEntity(dynamic_cast<cGameObject*>(entity));
}

iObject* cEntityManager::GetObjectByTag(std::string tag)
{
	for (unsigned int i = 0; i < Entities->size(); ++i)
	{
		if ((*Entities)[i]->tag == tag) return (*Entities)[i];
	}
	return nullptr;
}

cGameObject* cEntityManager::GetGameObjectByTag(std::string tag)
{
	for (unsigned int i = 0; i < Entities->size(); ++i)
	{
		if ((*Entities)[i]->tag == tag) return (*Entities)[i];
	}
	return nullptr;
}

void cEntityManager::SetEntities(std::vector<cGameObject*>* entities)
{
	delete Entities;
	Entities = entities;
}

void cEntityManager::Update(float dt)
{
	while (!EntityManager::remove_queue.empty())
	{
		iObject* obj = EntityManager::remove_queue.front();
		auto it = std::find(Entities->begin(), Entities->end(), obj);
		if (it != Entities->end())
		{
			Entities->erase(it);
			delete obj;
		}
		EntityManager::remove_queue.pop();
	}
}
