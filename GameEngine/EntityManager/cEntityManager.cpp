#include "cEntityManager.h"
#include <queue>

namespace EntityManager
{
	std::queue<iObject*> remove_queue;
}

cEntityManager::cEntityManager()
{
}

cEntityManager::~cEntityManager()
{
	for (size_t i = 0; i < Entities.size(); ++i)
	{
		delete Entities[i];
	}
	Entities.clear();
}

bool cEntityManager::AddEntity(iObject* entity)
{
	auto it = std::find(Entities.begin(), Entities.end(), entity);
	if (it == Entities.end())
	{
		Entities.push_back(entity);
		return true;
	}
	return false;
}

bool cEntityManager::RemoveEntity(iObject* entity)
{
	auto it = std::find(Entities.begin(), Entities.end(), entity);
	if (it != Entities.end())
	{
		EntityManager::remove_queue.push(entity);
		return true;
	}
	return false;
}

void cEntityManager::Update(float dt)
{
	while (!EntityManager::remove_queue.empty())
	{
		iObject* obj = EntityManager::remove_queue.front();
		auto it = std::find(Entities.begin(), Entities.end(), obj);
		if (it != Entities.end())
		{
			Entities.erase(it);
			delete obj;
		}
		EntityManager::remove_queue.pop();
	}
}
